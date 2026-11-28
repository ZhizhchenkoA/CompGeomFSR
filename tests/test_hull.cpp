#include <algorithm>

#include <QtTest>

#include "geometry/convex_hull.h"
#include "geometry/predicates.h"
#include "test_helpers.h"

using geometry::Point;

class HullTests : public QObject {
    Q_OBJECT
private slots:
    void emptyAndMinimalSets()
    {
        QVERIFY(geometry::convexHull({}).empty());
        const auto one = geometry::convexHull({{3, 4}, {3, 4}});
        QCOMPARE(one.size(), std::size_t{1});
        test_helpers::verifyPoint(one.front(), {3, 4});
        const auto two = geometry::convexHull({{2, 0}, {0, 0}});
        QCOMPARE(two.size(), std::size_t{2});
    }

    void squareExcludesInteriorAndDuplicatePoints()
    {
        const auto hull = geometry::convexHull({{0, 0}, {4, 0}, {4, 4}, {0, 4},
                                                {2, 2}, {0, 0}, {2, 0}});
        QCOMPARE(hull.size(), std::size_t{4});
        for (const Point corner : {Point{0, 0}, Point{4, 0}, Point{4, 4}, Point{0, 4}})
            QVERIFY(test_helpers::contains(hull, corner));
        QVERIFY(!test_helpers::contains(hull, {2, 2}));
        for (std::size_t i = 0; i < hull.size(); ++i)
            QVERIFY(geometry::cross(hull[i], hull[(i + 1) % hull.size()],
                                    hull[(i + 2) % hull.size()]) > 0);
    }

    void collinearPointsKeepOnlyExtremes()
    {
        const auto hull = geometry::convexHull({{3, 3}, {0, 0}, {2, 2}, {1, 1}, {2, 2}});
        QCOMPARE(hull.size(), std::size_t{2});
        QVERIFY(test_helpers::contains(hull, {0, 0}));
        QVERIFY(test_helpers::contains(hull, {3, 3}));
    }

    void permutationAndTranslationKeepHull()
    {
        std::vector<Point> points{{0, 0}, {3, 0}, {4, 2}, {2, 4}, {-1, 2}, {1, 1}};
        const auto baseline = geometry::convexHull(points);
        std::reverse(points.begin(), points.end());
        for (auto& point : points) { point.x += 17; point.y -= 9; }
        const auto moved = geometry::convexHull(points);
        QCOMPARE(moved.size(), baseline.size());
        for (const auto& vertex : baseline)
            QVERIFY(test_helpers::contains(moved, {vertex.x + 17, vertex.y - 9}));
    }

    void toleranceCanRemoveAlmostCollinearPoint()
    {
        const std::vector<Point> points{{0, 0}, {1, 0.001}, {2, 0}};
        QCOMPARE(geometry::convexHull(points, 0).size(), std::size_t{3});
        const auto simplified = geometry::convexHull(points, 0.003);
        QCOMPARE(simplified.size(), std::size_t{2});
        QVERIFY(test_helpers::contains(simplified, {0, 0}));
        QVERIFY(test_helpers::contains(simplified, {2, 0}));
    }

    void everyPermutationOfFivePointsHasSameHull()
    {
        std::vector<Point> points{{0, 0}, {3, 0}, {3, 3}, {0, 3}, {1, 1}};
        const auto expected = geometry::convexHull(points);
        std::sort(points.begin(), points.end(), [](const Point& a, const Point& b) {
            return a.x < b.x || (a.x == b.x && a.y < b.y);
        });
        do {
            const auto actual = geometry::convexHull(points);
            QCOMPARE(actual.size(), expected.size());
            for (const auto& vertex : expected)
                QVERIFY(test_helpers::contains(actual, vertex));
        } while (std::next_permutation(points.begin(), points.end(),
                                       [](const Point& a, const Point& b) {
                                           return a.x < b.x || (a.x == b.x && a.y < b.y);
                                       }));
    }
};

QTEST_APPLESS_MAIN(HullTests)
#include "test_hull.moc"
