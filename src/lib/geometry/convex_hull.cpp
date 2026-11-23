#include "geometry/convex_hull.h"

#include <algorithm>
#include <cmath>

#include "geometry/predicates.h"

namespace geometry {


bool samePoint(const Point& first, const Point& second, double eps)
{
    return std::abs(first.x - second.x) <= eps && std::abs(first.y - second.y) <= eps;
}



std::vector<Point> convexHull(std::vector<Point> points, double eps)
{
    const double tolerance = std::abs(eps);
    std::sort(points.begin(), points.end(), [](const Point& first, const Point& second) {
        return first.x < second.x || (first.x == second.x && first.y < second.y);
    });
    points.erase(std::unique(points.begin(), points.end(), [tolerance](const Point& first,
                                                                        const Point& second) {
        return samePoint(first, second, tolerance);
    }), points.end());

    if (points.size() <= 2) {
        return points;
    }

    std::vector<Point> hull;
    hull.reserve(points.size() * 2);
    for (const Point& point : points) {
        while (hull.size() >= 2 && cross(hull[hull.size() - 2], hull.back(), point) <= tolerance) {
            hull.pop_back();
        }
        hull.push_back(point);
    }

    const std::size_t lowerSize = hull.size();
    for (auto it = points.rbegin() + 1; it != points.rend(); ++it) {
        while (hull.size() > lowerSize && cross(hull[hull.size() - 2], hull.back(), *it) <= tolerance) {
            hull.pop_back();
        }
        hull.push_back(*it);
    }

    hull.pop_back(); // первая вершина нижней цепочки повторена в конце верхней
    return hull;
}

} // namespace geometry
