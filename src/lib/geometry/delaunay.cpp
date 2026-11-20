#include "geometry/delaunay.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <utility>

#include "geometry/predicates.h"

namespace geometry {
namespace {

struct IndexedTriangle {
    int first;
    int second;
    int third;
};

double squaredDistance(const Point& first, const Point& second)
{
    const double dx = first.x - second.x;
    const double dy = first.y - second.y;
    return dx * dx + dy * dy;
}

bool samePoint(const Point& first, const Point& second, double eps)
{
    return std::abs(first.x - second.x) <= eps && std::abs(first.y - second.y) <= eps;
}

IndexedTriangle makeCounterClockwise(int first, int second, int third,
                                     const std::vector<Point>& points)
{
    if (cross(points[first], points[second], points[third]) < 0.0) {
        std::swap(second, third);
    }
    return {first, second, third};
}

bool circleForTriangle(const IndexedTriangle& triangle, const std::vector<Point>& points,
                       Circumcircle* circle)
{
    const Point& a = points[triangle.first];
    const Point& b = points[triangle.second];
    const Point& c = points[triangle.third];
    const double denominator = 2.0 * cross(a, b, c);
    if (std::abs(denominator) <= 1e-15) {
        return false;
    }

    const double a2 = a.x * a.x + a.y * a.y;
    const double b2 = b.x * b.x + b.y * b.y;
    const double c2 = c.x * c.x + c.y * c.y;
    circle->center.x = (a2 * (b.y - c.y) + b2 * (c.y - a.y) + c2 * (a.y - b.y)) / denominator;
    circle->center.y = (a2 * (c.x - b.x) + b2 * (a.x - c.x) + c2 * (b.x - a.x)) / denominator;
    circle->radius = std::sqrt(squaredDistance(circle->center, a));
    return true;
}

bool containsInCircumcircle(const IndexedTriangle& triangle, const Point& point,
                            const std::vector<Point>& points)
{
    Circumcircle circle;
    if (!circleForTriangle(triangle, points, &circle)) {
        return false;
    }
    const double radiusSquared = circle.radius * circle.radius;
    // Малый относительный допуск сохраняет точки на окружности: один из
    // допустимых диагоналей остаётся, но внутренняя точка не пропускается.
    return squaredDistance(circle.center, point) <= radiusSquared +
           1e-10 * std::max(1.0, radiusSquared);
}

} // namespace

std::vector<DelaunayTriangle> delaunayTriangulation(std::vector<Point> points, double eps)
{
    const double tolerance = std::abs(eps);
    std::sort(points.begin(), points.end(), [](const Point& first, const Point& second) {
        return first.x < second.x || (first.x == second.x && first.y < second.y);
    });
    points.erase(std::unique(points.begin(), points.end(), [tolerance](const Point& first,
                                                                        const Point& second) {
        return samePoint(first, second, tolerance);
    }), points.end());
    if (points.size() < 3) {
        return {};
    }

    double minX = points.front().x;
    double maxX = minX;
    double minY = points.front().y;
    double maxY = minY;
    for (const Point& point : points) {
        minX = std::min(minX, point.x);
        maxX = std::max(maxX, point.x);
        minY = std::min(minY, point.y);
        maxY = std::max(maxY, point.y);
    }
    const double span = std::max(maxX - minX, maxY - minY);
    if (span <= tolerance) {
        return {};
    }
    const Point middle{(minX + maxX) / 2.0, (minY + maxY) / 2.0};
    const int originalCount = static_cast<int>(points.size());
    points.push_back({middle.x - 32.0 * span, middle.y - span});
    points.push_back({middle.x + 32.0 * span, middle.y - span});
    points.push_back({middle.x, middle.y + 32.0 * span});

    std::vector<IndexedTriangle> triangles;
    triangles.push_back(makeCounterClockwise(originalCount, originalCount + 1,
                                             originalCount + 2, points));

    for (int pointIndex = 0; pointIndex < originalCount; ++pointIndex) {
        std::vector<bool> bad(triangles.size(), false);
        std::map<std::pair<int, int>, int> edgeCounts;
        for (std::size_t i = 0; i < triangles.size(); ++i) {
            if (!containsInCircumcircle(triangles[i], points[pointIndex], points)) {
                continue;
            }
            bad[i] = true;
            const IndexedTriangle& triangle = triangles[i];
            const std::pair<int, int> edges[] = {
                std::minmax(triangle.first, triangle.second),
                std::minmax(triangle.second, triangle.third),
                std::minmax(triangle.third, triangle.first),
            };
            for (const auto& edge : edges) {
                ++edgeCounts[edge];
            }
        }

        std::vector<IndexedTriangle> next;
        next.reserve(triangles.size() + edgeCounts.size());
        for (std::size_t i = 0; i < triangles.size(); ++i) {
            if (!bad[i]) {
                next.push_back(triangles[i]);
            }
        }
        for (const auto& [edge, count] : edgeCounts) {
            if (count == 1) {
                next.push_back(makeCounterClockwise(edge.first, edge.second, pointIndex, points));
            }
        }
        triangles = std::move(next);
    }

    std::vector<DelaunayTriangle> result;
    for (const IndexedTriangle& triangle : triangles) {
        if (triangle.first >= originalCount || triangle.second >= originalCount ||
            triangle.third >= originalCount) {
            continue;
        }
        Circumcircle circle;
        if (!circleForTriangle(triangle, points, &circle)) {
            continue;
        }
        result.push_back({{points[triangle.first], points[triangle.second], points[triangle.third]}, circle});
    }
    return result;
}

} // namespace geometry
