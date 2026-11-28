#include <QtTest>

#include "geometry/predicates.h"
#include "test_helpers.h"

using geometry::Point;

class PredicateTests : public QObject {
    Q_OBJECT
private slots:
    void turnCases_data()
    {
        QTest::addColumn<Point>("a");
        QTest::addColumn<Point>("b");
        QTest::addColumn<Point>("p");
        QTest::addColumn<int>("expected");
        QTest::newRow("left") << Point{0, 0} << Point{4, 0} << Point{2, 3} << 1;
        QTest::newRow("right") << Point{0, 0} << Point{4, 0} << Point{2, -3} << -1;
        QTest::newRow("collinear") << Point{0, 0} << Point{4, 0} << Point{7, 0} << 0;
        QTest::newRow("coincident-ends") << Point{1, 1} << Point{1, 1} << Point{5, 8} << 0;
    }

    void turnCases()
    {
        QFETCH(Point, a); QFETCH(Point, b); QFETCH(Point, p); QFETCH(int, expected);
        QCOMPARE(geometry::turn(a, b, p), expected);
    }

    void reversingDirectionFlipsSign()
    {
        const Point a{1, 2}, b{5, 3}, p{2, 7};
        QCOMPARE(geometry::turn(a, b, p), -geometry::turn(b, a, p));
        QCOMPARE(geometry::turn(a, b, p), 1);
    }

    void epsilonControlsNearCollinearPoint()
    {
        const Point a{0, 0}, b{10, 0}, p{5, 0.001};
        QCOMPARE(geometry::turn(a, b, p, 0.02), 0);
        QCOMPARE(geometry::turn(a, b, p, -0.02), 0);
        QCOMPARE(geometry::turn(a, b, p, 0.001), 1);
    }

    void onSegmentCases_data()
    {
        QTest::addColumn<Point>("a"); QTest::addColumn<Point>("b");
        QTest::addColumn<Point>("p"); QTest::addColumn<bool>("expected");
        QTest::newRow("interior") << Point{0, 0} << Point{4, 4} << Point{2, 2} << true;
        QTest::newRow("endpoint") << Point{0, 0} << Point{4, 4} << Point{4, 4} << true;
        QTest::newRow("collinear-outside") << Point{0, 0} << Point{4, 4} << Point{5, 5} << false;
        QTest::newRow("off-line") << Point{0, 0} << Point{4, 4} << Point{2, 3} << false;
        QTest::newRow("zero-length-hit") << Point{1, 1} << Point{1, 1} << Point{1, 1} << true;
        QTest::newRow("zero-length-miss") << Point{1, 1} << Point{1, 1} << Point{2, 1} << false;
    }

    void onSegmentCases()
    {
        QFETCH(Point, a); QFETCH(Point, b); QFETCH(Point, p); QFETCH(bool, expected);
        QCOMPARE(geometry::onSegment(a, b, p), expected);
    }

    void onSegmentUsesDistanceTolerance()
    {
        QVERIFY(geometry::onSegment({0, 0}, {10, 0}, {5, 0.5}, 0.5));
        QVERIFY(!geometry::onSegment({0, 0}, {10, 0}, {5, 0.51}, 0.5));
    }

    void endpointAndDegenerateSegmentUseCircularTolerance()
    {
        QVERIFY(geometry::onSegment({0, 0}, {10, 0}, {-3, 4}, 5));
        QVERIFY(!geometry::onSegment({0, 0}, {10, 0}, {-3, 4}, 4.99));
        QVERIFY(geometry::onSegment({2, 2}, {2, 2}, {5, 6}, 5));
        QVERIFY(!geometry::onSegment({2, 2}, {2, 2}, {5, 6}, 4.99));
    }

    void translationKeepsTurnAndSegmentMembership()
    {
        const Point a{0, 0}, b{10, 0}, p{4, 3}, on{5, 0};
        const Point shift{1000000, -2000000};
        const auto moved = [shift](Point point) {
            return Point{point.x + shift.x, point.y + shift.y};
        };
        QCOMPARE(geometry::turn(moved(a), moved(b), moved(p)), geometry::turn(a, b, p));
        QCOMPARE(geometry::onSegment(moved(a), moved(b), moved(on)),
                 geometry::onSegment(a, b, on));
    }
};

QTEST_APPLESS_MAIN(PredicateTests)
#include "test_predicates.moc"
