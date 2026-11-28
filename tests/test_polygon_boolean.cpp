#include <algorithm>
#include <cmath>

#include <QtTest>

#include "geometry/convex_polygon_boolean.h"

using geometry::ConvexPolygonOperation;
using geometry::Point;
using geometry::PolygonSet;

namespace {

std::vector<Point> rectangle(double left, double bottom, double right, double top)
{
    return {{left, bottom}, {right, bottom}, {right, top}, {left, top}};
}

double signedArea(const std::vector<Point>& polygon)
{
    double twiceArea = 0;
    for (std::size_t i = 0; i < polygon.size(); ++i) {
        const Point& a = polygon[i];
        const Point& b = polygon[(i + 1) % polygon.size()];
        twiceArea += a.x * b.y - b.x * a.y;
    }
    return twiceArea / 2;
}

double totalArea(const PolygonSet& polygons)
{
    double area = 0;
    for (const auto& polygon : polygons) area += signedArea(polygon);
    return area;
}

} // namespace

class PolygonBooleanTests : public QObject {
    Q_OBJECT
private slots:
    void overlappingRectanglesSatisfyAreaIdentities()
    {
        const auto a = rectangle(0, 0, 4, 3);
        const auto b = rectangle(2, 1, 6, 4);
        const auto intersection = geometry::convexPolygonBoolean(
            a, b, ConvexPolygonOperation::Intersection);
        const auto united = geometry::convexPolygonBoolean(
            a, b, ConvexPolygonOperation::Union);
        const auto difference = geometry::convexPolygonBoolean(
            a, b, ConvexPolygonOperation::Difference);

        QCOMPARE(intersection.size(), std::size_t{1});
        QCOMPARE(united.size(), std::size_t{1});
        QCOMPARE(difference.size(), std::size_t{1});
        QVERIFY(std::abs(totalArea(intersection) - 4) < 1e-8);
        QVERIFY(std::abs(totalArea(united) - 20) < 1e-8);
        QVERIFY(std::abs(totalArea(difference) - 8) < 1e-8);
        QVERIFY(std::abs(totalArea(united) + totalArea(intersection) - 24) < 1e-8);
        QVERIFY(std::abs(totalArea(difference) + totalArea(intersection) - 12) < 1e-8);
    }

    void disjointRectanglesRemainSeparate()
    {
        const auto a = rectangle(0, 0, 2, 2);
        const auto b = rectangle(4, 0, 6, 2);
        const auto intersection = geometry::convexPolygonBoolean(
            a, b, ConvexPolygonOperation::Intersection);
        const auto united = geometry::convexPolygonBoolean(
            a, b, ConvexPolygonOperation::Union);
        const auto difference = geometry::convexPolygonBoolean(
            a, b, ConvexPolygonOperation::Difference);

        QVERIFY(intersection.empty());
        QCOMPARE(united.size(), std::size_t{2});
        QCOMPARE(difference.size(), std::size_t{1});
        QVERIFY(std::abs(totalArea(united) - 8) < 1e-8);
        QVERIFY(std::abs(totalArea(difference) - 4) < 1e-8);
    }

    void containedRectangleProducesHole()
    {
        const auto outer = rectangle(0, 0, 5, 5);
        const auto inner = rectangle(1, 1, 3, 3);
        const auto intersection = geometry::convexPolygonBoolean(
            outer, inner, ConvexPolygonOperation::Intersection);
        const auto united = geometry::convexPolygonBoolean(
            outer, inner, ConvexPolygonOperation::Union);
        const auto difference = geometry::convexPolygonBoolean(
            outer, inner, ConvexPolygonOperation::Difference);

        QVERIFY(std::abs(totalArea(intersection) - 4) < 1e-8);
        QVERIFY(std::abs(totalArea(united) - 25) < 1e-8);
        QCOMPARE(difference.size(), std::size_t{2});
        QVERIFY(std::abs(totalArea(difference) - 21) < 1e-8);
        QVERIFY(signedArea(difference[0]) * signedArea(difference[1]) < 0);
    }

    void edgeTouchDoesNotCreateAreaIntersection()
    {
        const auto a = rectangle(0, 0, 2, 2);
        const auto b = rectangle(2, 0, 4, 2);
        const auto intersection = geometry::convexPolygonBoolean(
            a, b, ConvexPolygonOperation::Intersection);
        const auto united = geometry::convexPolygonBoolean(
            a, b, ConvexPolygonOperation::Union);
        const auto difference = geometry::convexPolygonBoolean(
            a, b, ConvexPolygonOperation::Difference);

        QVERIFY(intersection.empty());
        QCOMPARE(united.size(), std::size_t{1});
        QCOMPARE(difference.size(), std::size_t{1});
        QVERIFY(std::abs(totalArea(united) - 8) < 1e-8);
        QVERIFY(std::abs(totalArea(difference) - 4) < 1e-8);
        for (const auto& contour : united) {
            for (std::size_t i = 0; i < contour.size(); ++i) {
                const Point& a = contour[i];
                const Point& b = contour[(i + 1) % contour.size()];
                const Point middle{(a.x + b.x) / 2, (a.y + b.y) / 2};
                QVERIFY2(!(std::abs(middle.x - 2) < 1e-8 &&
                           middle.y > 0 && middle.y < 2),
                         "The internal common edge must not appear in the union boundary");
            }
        }
    }

    void pointTouchDoesNotCreateAreaIntersection()
    {
        const auto a = rectangle(0, 0, 2, 2);
        const auto b = rectangle(2, 2, 4, 4);
        const auto intersection = geometry::convexPolygonBoolean(
            a, b, ConvexPolygonOperation::Intersection);
        const auto united = geometry::convexPolygonBoolean(
            a, b, ConvexPolygonOperation::Union);

        QVERIFY(intersection.empty());
        QVERIFY(std::abs(totalArea(united) - 8) < 1e-8);
    }

    void identicalPolygonsObeySetIdentities()
    {
        const auto square = rectangle(0, 0, 3, 3);
        const auto intersection = geometry::convexPolygonBoolean(
            square, square, ConvexPolygonOperation::Intersection);
        const auto united = geometry::convexPolygonBoolean(
            square, square, ConvexPolygonOperation::Union);
        const auto difference = geometry::convexPolygonBoolean(
            square, square, ConvexPolygonOperation::Difference);
        QVERIFY(std::abs(totalArea(intersection) - 9) < 1e-8);
        QVERIFY(std::abs(totalArea(united) - 9) < 1e-8);
        QVERIFY(difference.empty());
    }

    void partiallySharedOuterEdgeIsNotLost()
    {
        const auto a = rectangle(0, 0, 3, 2);
        const auto b = rectangle(1, 0, 4, 2);
        const auto intersection = geometry::convexPolygonBoolean(
            a, b, ConvexPolygonOperation::Intersection);
        const auto united = geometry::convexPolygonBoolean(
            a, b, ConvexPolygonOperation::Union);
        const auto difference = geometry::convexPolygonBoolean(
            a, b, ConvexPolygonOperation::Difference);
        QVERIFY(std::abs(totalArea(intersection) - 4) < 1e-8);
        QVERIFY(std::abs(totalArea(united) - 8) < 1e-8);
        QVERIFY(std::abs(totalArea(difference) - 2) < 1e-8);
    }

    void diagonalSharedEdgeFormsFullSquareOnUnion()
    {
        const std::vector<Point> a{{0, 0}, {4, 0}, {0, 4}};
        const std::vector<Point> b{{4, 0}, {4, 4}, {0, 4}};
        const auto intersection = geometry::convexPolygonBoolean(
            a, b, ConvexPolygonOperation::Intersection);
        const auto united = geometry::convexPolygonBoolean(
            a, b, ConvexPolygonOperation::Union);
        QVERIFY(intersection.empty());
        QCOMPARE(united.size(), std::size_t{1});
        QVERIFY(std::abs(totalArea(united) - 16) < 1e-8);
    }

    void emptyOperandFollowsSetIdentities()
    {
        const auto square = rectangle(0, 0, 2, 2);
        QVERIFY(geometry::convexPolygonBoolean(
            square, {}, ConvexPolygonOperation::Intersection).empty());
        QVERIFY(geometry::convexPolygonBoolean(
            {}, square, ConvexPolygonOperation::Difference).empty());
        QVERIFY(std::abs(totalArea(geometry::convexPolygonBoolean(
            square, {}, ConvexPolygonOperation::Union)) - 4) < 1e-8);
        QVERIFY(std::abs(totalArea(geometry::convexPolygonBoolean(
            square, {}, ConvexPolygonOperation::Difference)) - 4) < 1e-8);
    }

    void allSmallIntegerRectanglePairsHaveCorrectAreas()
    {
        struct Box { int left, bottom, right, top; };
        std::vector<Box> boxes;
        for (int left = 0; left < 3; ++left)
            for (int right = left + 1; right <= 3; ++right)
                for (int bottom = 0; bottom < 3; ++bottom)
                    for (int top = bottom + 1; top <= 3; ++top)
                        boxes.push_back({left, bottom, right, top});
        QCOMPARE(boxes.size(), std::size_t{36});

        for (const Box& a : boxes) {
            for (const Box& b : boxes) {
                const auto first = rectangle(a.left, a.bottom, a.right, a.top);
                const auto second = rectangle(b.left, b.bottom, b.right, b.top);
                const double areaA = (a.right - a.left) * (a.top - a.bottom);
                const double areaB = (b.right - b.left) * (b.top - b.bottom);
                const double overlapWidth = std::max(0, std::min(a.right, b.right) -
                                                        std::max(a.left, b.left));
                const double overlapHeight = std::max(0, std::min(a.top, b.top) -
                                                         std::max(a.bottom, b.bottom));
                const double overlap = overlapWidth * overlapHeight;
                const auto intersection = geometry::convexPolygonBoolean(
                    first, second, ConvexPolygonOperation::Intersection);
                const auto united = geometry::convexPolygonBoolean(
                    first, second, ConvexPolygonOperation::Union);
                const auto difference = geometry::convexPolygonBoolean(
                    first, second, ConvexPolygonOperation::Difference);
                const QByteArray description = QString(
                    "A=(%1,%2)-(%3,%4), B=(%5,%6)-(%7,%8)")
                    .arg(a.left).arg(a.bottom).arg(a.right).arg(a.top)
                    .arg(b.left).arg(b.bottom).arg(b.right).arg(b.top).toUtf8();
                QVERIFY2(std::abs(totalArea(intersection) - overlap) < 1e-8,
                         description.constData());
                QVERIFY2(std::abs(totalArea(united) - (areaA + areaB - overlap)) < 1e-8,
                         description.constData());
                QVERIFY2(std::abs(totalArea(difference) - (areaA - overlap)) < 1e-8,
                         description.constData());
            }
        }
    }
};

QTEST_APPLESS_MAIN(PolygonBooleanTests)
#include "test_polygon_boolean.moc"
