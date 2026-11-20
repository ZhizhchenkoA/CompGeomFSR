#include "geometry/predicates.h"

#include <algorithm>
#include <cmath>

namespace geometry {

double cross(const Point& a, const Point& b, const Point& c)
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

int turn(const Point& a, const Point& b, const Point& p, double eps)
{
    const double value = cross(a, b, p);
    const double tolerance = std::abs(eps);
    if (value > tolerance) {
        return 1;
    }
    if (value < -tolerance) {
        return -1;
    }
    return 0;
}

bool onSegment(const Point& a, const Point& b, const Point& p, double eps)
{
    const double tolerance = std::abs(eps);
    // Вырожденный отрезок — одна точка. В этом случае проверяем
    // расстояние до неё.
    const double dx = b.x - a.x;
    const double dy = b.y - a.y;
    const double lengthSquared = dx * dx + dy * dy;
    if (lengthSquared <= tolerance * tolerance) {
        const double px = p.x - a.x;
        const double py = p.y - a.y;
        return px * px + py * py <= tolerance * tolerance;
    }

    // Проецируем P на AB и ограничиваем проекцию концами отрезка.
    // В отличие от допуска к cross, это расстояние имеет те же единицы,
    // что и координаты, поэтому удобно для экранного радиуса точки.
    const double projection = ((p.x - a.x) * dx + (p.y - a.y) * dy) / lengthSquared;
    const double t = std::clamp(projection, 0.0, 1.0);
    const double closestX = a.x + t * dx;
    const double closestY = a.y + t * dy;
    const double distanceX = p.x - closestX;
    const double distanceY = p.y - closestY;
    return distanceX * distanceX + distanceY * distanceY <= tolerance * tolerance;
}
} 
