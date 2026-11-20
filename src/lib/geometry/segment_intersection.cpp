#include "geometry/segment_intersection.h"

#include "geometry/predicates.h"

#include <algorithm>
#include <cmath>

namespace geometry {
namespace {

Point subtract(const Point& lhs, const Point& rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

Point addScaled(const Point& origin, const Point& direction, double t)
{
    return {origin.x + direction.x * t, origin.y + direction.y * t};
}

double crossVectors(const Point& lhs, const Point& rhs)
{
    return lhs.x * rhs.y - lhs.y * rhs.x;
}

double dot(const Point& lhs, const Point& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

double length(const Point& vector)
{
    return std::hypot(vector.x, vector.y);
}

} // namespace

SegmentIntersection intersectSegments(const Point& a, const Point& b,
                                      const Point& c, const Point& d, double eps)
{
    const double tolerance = std::abs(eps);
    const Point r = subtract(b, a);
    const Point s = subtract(d, c);
    const double rLength = length(r);
    const double sLength = length(s);

    // Вырожденные отрезки: точка-точка и точка-отрезок.
    if (rLength <= tolerance && sLength <= tolerance) {
        return onSegment(a, a, c, tolerance)
            ? SegmentIntersection{SegmentIntersectionKind::Point, a, {}}
            : SegmentIntersection{};
    }
    if (rLength <= tolerance) {
        return onSegment(c, d, a, tolerance)
            ? SegmentIntersection{SegmentIntersectionKind::Point, a, {}}
            : SegmentIntersection{};
    }
    if (sLength <= tolerance) {
        return onSegment(a, b, c, tolerance)
            ? SegmentIntersection{SegmentIntersectionKind::Point, c, {}}
            : SegmentIntersection{};
    }

    const Point cMinusA = subtract(c, a);
    const double rCrossS = crossVectors(r, s);
    // cross(r, s) имеет размерность площади; допускаем отклонение,
    // соответствующее полосе шириной tolerance около более длинного отрезка.
    const double parallelTolerance = tolerance * std::max(rLength, sLength);

    if (std::abs(rCrossS) <= parallelTolerance) {
        // Параллельные отрезки пересекаются только при коллинеарности.
        if (std::abs(crossVectors(cMinusA, r)) > tolerance * rLength) {
            return {};
        }

        const double rLengthSquared = dot(r, r);
        const double tC = dot(cMinusA, r) / rLengthSquared;
        const double tD = dot(subtract(d, a), r) / rLengthSquared;
        const double from = std::max(0.0, std::min(tC, tD));
        const double to = std::min(1.0, std::max(tC, tD));
        const double parameterTolerance = tolerance / rLength;
        if (from > to + parameterTolerance) {
            return {};
        }
        if (std::abs(to - from) <= parameterTolerance) {
            return {SegmentIntersectionKind::Point, addScaled(a, r, (from + to) / 2.0), {}};
        }
        return {SegmentIntersectionKind::Overlap, addScaled(a, r, from), addScaled(a, r, to)};
    }

    const double t = crossVectors(cMinusA, s) / rCrossS;
    const double u = crossVectors(cMinusA, r) / rCrossS;
    const double tTolerance = tolerance / rLength;
    const double uTolerance = tolerance / sLength;
    if (t < -tTolerance || t > 1.0 + tTolerance ||
        u < -uTolerance || u > 1.0 + uTolerance) {
        return {};
    }
    return {SegmentIntersectionKind::Point, addScaled(a, r, std::clamp(t, 0.0, 1.0)), {}};
}

} // namespace geometry
