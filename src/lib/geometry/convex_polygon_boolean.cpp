#include "geometry/convex_polygon_boolean.h"

#include <algorithm>
#include <cmath>

#include "geometry/predicates.h"
#include "geometry/segment_intersection.h"

namespace geometry {
namespace {

struct Fragment { Point from; Point to; };

double squaredDistance(const Point& first, const Point& second)
{
    const double dx = first.x - second.x;
    const double dy = first.y - second.y;
    return dx * dx + dy * dy;
}

bool samePoint(const Point& first, const Point& second, double eps)
{
    return squaredDistance(first, second) <= eps * eps;
}

bool insideOrOn(const std::vector<Point>& polygon, const Point& point, double eps)
{
    if (polygon.size() < 3) {
        return false;
    }
    for (std::size_t i = 0; i < polygon.size(); ++i) {
        if (cross(polygon[i], polygon[(i + 1) % polygon.size()], point) < -eps) {
            return false;
        }
    }
    return true;
}

bool strictlyInside(const std::vector<Point>& polygon, const Point& point, double eps)
{
    if (!insideOrOn(polygon, point, eps)) {
        return false;
    }
    for (std::size_t i = 0; i < polygon.size(); ++i) {
        if (onSegment(polygon[i], polygon[(i + 1) % polygon.size()], point, eps)) {
            return false;
        }
    }
    return true;
}

double parameterOnEdge(const Point& first, const Point& second, const Point& point)
{
    const double dx = second.x - first.x;
    const double dy = second.y - first.y;
    const double lengthSquared = dx * dx + dy * dy;
    return lengthSquared == 0.0 ? 0.0 : ((point.x - first.x) * dx + (point.y - first.y) * dy) / lengthSquared;
}

std::vector<Fragment> splitEdges(const std::vector<Point>& polygon,
                                 const std::vector<Point>& other, double eps)
{
    std::vector<Fragment> fragments;
    if (polygon.size() < 3 || other.size() < 3) {
        return fragments;
    }
    for (std::size_t i = 0; i < polygon.size(); ++i) {
        const Point& first = polygon[i];
        const Point& second = polygon[(i + 1) % polygon.size()];
        std::vector<double> parameters{0.0, 1.0};
        for (std::size_t j = 0; j < other.size(); ++j) {
            const SegmentIntersection intersection = intersectSegments(
                first, second, other[j], other[(j + 1) % other.size()], eps);
            if (intersection.kind == SegmentIntersectionKind::Point) {
                parameters.push_back(std::clamp(parameterOnEdge(first, second, intersection.first), 0.0, 1.0));
            } else if (intersection.kind == SegmentIntersectionKind::Overlap) {
                parameters.push_back(std::clamp(parameterOnEdge(first, second, intersection.first), 0.0, 1.0));
                parameters.push_back(std::clamp(parameterOnEdge(first, second, intersection.second), 0.0, 1.0));
            }
        }
        std::sort(parameters.begin(), parameters.end());
        parameters.erase(std::unique(parameters.begin(), parameters.end(), [eps](double left, double right) {
            return std::abs(left - right) <= eps;
        }), parameters.end());
        for (std::size_t j = 1; j < parameters.size(); ++j) {
            const double from = parameters[j - 1];
            const double to = parameters[j];
            if (to - from <= eps) {
                continue;
            }
            fragments.push_back({{first.x + (second.x - first.x) * from,
                                  first.y + (second.y - first.y) * from},
                                 {first.x + (second.x - first.x) * to,
                                  first.y + (second.y - first.y) * to}});
        }
    }
    return fragments;
}

PolygonSet joinFragments(std::vector<Fragment> fragments, double eps)
{
    PolygonSet result;
    while (!fragments.empty()) {
        std::vector<Point> loop{fragments.back().from, fragments.back().to};
        fragments.pop_back();
        while (!samePoint(loop.back(), loop.front(), eps)) {
            auto next = std::find_if(fragments.begin(), fragments.end(), [&](const Fragment& fragment) {
                return samePoint(fragment.from, loop.back(), eps);
            });
            if (next == fragments.end()) {
                loop.clear(); // касание без площади не образует многоугольник
                break;
            }
            loop.push_back(next->to);
            fragments.erase(next);
        }
        if (loop.size() >= 4) {
            loop.pop_back();
            result.push_back(std::move(loop));
        }
    }
    return result;
}

} // namespace

PolygonSet convexPolygonBoolean(const std::vector<Point>& first, const std::vector<Point>& second,
                                 ConvexPolygonOperation operation, double eps)
{
    const double tolerance = std::abs(eps);
    if (first.size() < 3 || second.size() < 3) {
        if (operation == ConvexPolygonOperation::Union) {
            PolygonSet result;
            if (first.size() >= 3) result.push_back(first);
            if (second.size() >= 3) result.push_back(second);
            return result;
        }
        if (operation == ConvexPolygonOperation::Difference && first.size() >= 3) {
            return {first};
        }
        return {};
    }

    const std::vector<Fragment> firstParts = splitEdges(first, second, tolerance);
    const std::vector<Fragment> secondParts = splitEdges(second, first, tolerance);
    std::vector<Fragment> selected;
    const auto selectParts = [&](const std::vector<Fragment>& fragments, const std::vector<Point>& other,
                                 int selection, bool reverse) {
        for (const Fragment& fragment : fragments) {
            const Point middle{(fragment.from.x + fragment.to.x) / 2.0,
                               (fragment.from.y + fragment.to.y) / 2.0};
            const bool strict = strictlyInside(other, middle, tolerance);
            const bool onOrInside = insideOrOn(other, middle, tolerance);
            // 0: строго внутри, 1: внутри или на границе,
            // 2: снаружи или на границе, 3: строго снаружи.
            const bool keep = selection == 0 ? strict : selection == 1 ? onOrInside :
                              selection == 2 ? !strict : !onOrInside;
            if (!keep) {
                continue;
            }
            selected.push_back(reverse ? Fragment{fragment.to, fragment.from} : fragment);
        }
    };

    switch (operation) {
    case ConvexPolygonOperation::Intersection:
        // Общую границу берём только из первого контура, чтобы не дублировать её.
        selectParts(firstParts, second, 1, false);
        selectParts(secondParts, first, 0, false);
        break;
    case ConvexPolygonOperation::Union:
        selectParts(firstParts, second, 2, false);
        selectParts(secondParts, first, 3, false);
        break;
    case ConvexPolygonOperation::Difference:
        selectParts(firstParts, second, 3, false);
        selectParts(secondParts, first, 0, true);
        break;
    }
    return joinFragments(std::move(selected), tolerance);
}

} // namespace geometry
