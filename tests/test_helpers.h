#pragma once

#include <cmath>
#include <vector>

#include <QtTest>

#include "geometry/point.h"
#include "geometry/segment_intersection.h"

Q_DECLARE_METATYPE(geometry::Point)
Q_DECLARE_METATYPE(geometry::SegmentIntersectionKind)

namespace test_helpers {

inline void verifyPoint(const geometry::Point& actual, const geometry::Point& expected,
                        double tolerance = 1e-8)
{
    QVERIFY2(std::abs(actual.x - expected.x) <= tolerance, "Wrong x coordinate");
    QVERIFY2(std::abs(actual.y - expected.y) <= tolerance, "Wrong y coordinate");
}

inline bool contains(const std::vector<geometry::Point>& points,
                     const geometry::Point& candidate, double tolerance = 1e-8)
{
    for (const auto& point : points) {
        if (std::abs(point.x - candidate.x) <= tolerance &&
            std::abs(point.y - candidate.y) <= tolerance) return true;
    }
    return false;
}

} // namespace test_helpers
