#include <QtTest>

#include "geometry/predicates.h"
#include "geometry/segment_intersection.h"
#include "test_helpers.h"

using geometry::Point;
using geometry::SegmentIntersectionKind;

class SegmentTests : public QObject {
    Q_OBJECT
private slots:
    void intersectionCases_data()
    {
        QTest::addColumn<Point>("a"); QTest::addColumn<Point>("b");
        QTest::addColumn<Point>("c"); QTest::addColumn<Point>("d");
        QTest::addColumn<SegmentIntersectionKind>("kind");
        QTest::addColumn<Point>("first"); QTest::addColumn<Point>("second");
        QTest::newRow("proper-crossing") << Point{0, 0} << Point{4, 4} << Point{0, 4} << Point{4, 0}
            << SegmentIntersectionKind::Point << Point{2, 2} << Point{};
        QTest::newRow("endpoint-touch") << Point{0, 0} << Point{2, 0} << Point{2, 0} << Point{2, 3}
            << SegmentIntersectionKind::Point << Point{2, 0} << Point{};
        QTest::newRow("parallel-separated") << Point{0, 0} << Point{3, 0} << Point{0, 1} << Point{3, 1}
            << SegmentIntersectionKind::None << Point{} << Point{};
        QTest::newRow("collinear-disjoint") << Point{0, 0} << Point{2, 0} << Point{3, 0} << Point{5, 0}
            << SegmentIntersectionKind::None << Point{} << Point{};
        QTest::newRow("collinear-overlap") << Point{0, 0} << Point{4, 0} << Point{2, 0} << Point{6, 0}
            << SegmentIntersectionKind::Overlap << Point{2, 0} << Point{4, 0};
        QTest::newRow("collinear-endpoint-touch") << Point{0, 0} << Point{2, 0} << Point{2, 0} << Point{4, 0}
            << SegmentIntersectionKind::Point << Point{2, 0} << Point{};
        QTest::newRow("contained-segment") << Point{0, 0} << Point{6, 0} << Point{2, 0} << Point{4, 0}
            << SegmentIntersectionKind::Overlap << Point{2, 0} << Point{4, 0};
        QTest::newRow("vertical-overlap") << Point{1, 0} << Point{1, 5} << Point{1, 3} << Point{1, 7}
            << SegmentIntersectionKind::Overlap << Point{1, 3} << Point{1, 5};
        QTest::newRow("identical-reversed") << Point{4, 0} << Point{0, 0} << Point{0, 0} << Point{4, 0}
            << SegmentIntersectionKind::Overlap << Point{4, 0} << Point{0, 0};
        QTest::newRow("lines-cross-outside-segments") << Point{0, 0} << Point{2, 0} << Point{3, -1} << Point{3, 1}
            << SegmentIntersectionKind::None << Point{} << Point{};
        QTest::newRow("point-on-segment") << Point{2, 0} << Point{2, 0} << Point{0, 0} << Point{4, 0}
            << SegmentIntersectionKind::Point << Point{2, 0} << Point{};
        QTest::newRow("two-equal-points") << Point{2, 0} << Point{2, 0} << Point{2, 0} << Point{2, 0}
            << SegmentIntersectionKind::Point << Point{2, 0} << Point{};
        QTest::newRow("two-distinct-points") << Point{2, 0} << Point{2, 0} << Point{3, 0} << Point{3, 0}
            << SegmentIntersectionKind::None << Point{} << Point{};
    }

    void intersectionCases()
    {
        QFETCH(Point, a); QFETCH(Point, b); QFETCH(Point, c); QFETCH(Point, d);
        QFETCH(SegmentIntersectionKind, kind); QFETCH(Point, first); QFETCH(Point, second);
        const auto result = geometry::intersectSegments(a, b, c, d);
        QCOMPARE(result.kind, kind);
        if (kind != SegmentIntersectionKind::None) test_helpers::verifyPoint(result.first, first);
        if (kind == SegmentIntersectionKind::Overlap) test_helpers::verifyPoint(result.second, second);
    }

    void swappingSegmentsKeepsIntersection()
    {
        const auto ab = geometry::intersectSegments({0, 0}, {8, 0}, {4, -2}, {4, 2});
        const auto cd = geometry::intersectSegments({4, -2}, {4, 2}, {0, 0}, {8, 0});
        QCOMPARE(ab.kind, SegmentIntersectionKind::Point);
        QCOMPARE(cd.kind, ab.kind);
        test_helpers::verifyPoint(cd.first, ab.first);
    }

    void translationKeepsIntersectionPoint()
    {
        const auto original = geometry::intersectSegments({0, 0}, {4, 4}, {0, 4}, {4, 0});
        const auto moved = geometry::intersectSegments(
            {1000000, -1000000}, {1000004, -999996},
            {1000000, -999996}, {1000004, -1000000});
        QCOMPARE(original.kind, SegmentIntersectionKind::Point);
        QCOMPARE(moved.kind, original.kind);
        test_helpers::verifyPoint(moved.first,
                                  {original.first.x + 1000000, original.first.y - 1000000});
    }

    void allSmallGridPairsAreSymmetric()
    {
        const std::vector<Point> grid{{0, 0}, {0, 1}, {0, 2},
                                      {1, 0}, {1, 1}, {1, 2},
                                      {2, 0}, {2, 1}, {2, 2}};
        const auto equal = [](const Point& lhs, const Point& rhs) {
            return test_helpers::contains({lhs}, rhs);
        };
        for (const auto& a : grid)
            for (const auto& b : grid)
                for (const auto& c : grid)
                    for (const auto& d : grid) {
                        const auto forward = geometry::intersectSegments(a, b, c, d);
                        const auto backward = geometry::intersectSegments(c, d, a, b);
                        QCOMPARE(forward.kind, backward.kind);
                        if (forward.kind == SegmentIntersectionKind::Point) {
                            QVERIFY(equal(forward.first, backward.first));
                            QVERIFY(geometry::onSegment(a, b, forward.first));
                            QVERIFY(geometry::onSegment(c, d, forward.first));
                        }
                        if (forward.kind == SegmentIntersectionKind::Overlap) {
                            QVERIFY((equal(forward.first, backward.first) &&
                                     equal(forward.second, backward.second)) ||
                                    (equal(forward.first, backward.second) &&
                                     equal(forward.second, backward.first)));
                            QVERIFY(geometry::onSegment(a, b, forward.first));
                            QVERIFY(geometry::onSegment(c, d, forward.first));
                            QVERIFY(geometry::onSegment(a, b, forward.second));
                            QVERIFY(geometry::onSegment(c, d, forward.second));
                        }
                    }
    }
};

QTEST_APPLESS_MAIN(SegmentTests)
#include "test_segments.moc"
