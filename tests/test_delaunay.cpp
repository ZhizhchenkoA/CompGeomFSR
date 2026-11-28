#include <cmath>

#include <QtTest>

#include "geometry/delaunay.h"
#include "geometry/predicates.h"
#include "test_helpers.h"

using geometry::Point;

class DelaunayTests : public QObject {
    Q_OBJECT
private slots:
    void emptyDuplicatesAndCollinearInputs()
    {
        QVERIFY(geometry::delaunayTriangulation({}).empty());
        QVERIFY(geometry::delaunayTriangulation({{0, 0}, {1, 0}}).empty());
        QVERIFY(geometry::delaunayTriangulation({{0, 0}, {0, 0}, {1, 0}}).empty());
        QVERIFY(geometry::delaunayTriangulation({{0, 0}, {1, 0}, {2, 0}}).empty());
    }

    void threePointsProduceCounterClockwiseTriangle()
    {
        const auto triangles = geometry::delaunayTriangulation({{0, 0}, {0, 3}, {4, 0}});
        QCOMPARE(triangles.size(), std::size_t{1});
        const auto& triangle = triangles.front();
        QVERIFY(geometry::cross(triangle.vertices[0], triangle.vertices[1],
                                triangle.vertices[2]) > 0);
        test_helpers::verifyPoint(triangle.circumcircle.center, {2, 1.5});
        QVERIFY(std::abs(triangle.circumcircle.radius - 2.5) < 1e-8);
    }

    void interiorPointCannotLieInsideAnyFinalCircumcircle()
    {
        const std::vector<Point> input{{0, 0}, {5, 0}, {5, 5}, {0, 5}, {2, 1}};
        const auto triangles = geometry::delaunayTriangulation(input);
        QVERIFY(!triangles.empty());
        for (const auto& triangle : triangles) {
            QVERIFY(geometry::cross(triangle.vertices[0], triangle.vertices[1],
                                    triangle.vertices[2]) > 0);
            const auto& center = triangle.circumcircle.center;
            const double radius2 = triangle.circumcircle.radius * triangle.circumcircle.radius;
            for (const auto& point : input) {
                bool vertex = false;
                for (const auto& corner : triangle.vertices)
                    vertex |= test_helpers::contains({corner}, point);
                if (vertex) continue;
                const double dx = point.x - center.x, dy = point.y - center.y;
                QVERIFY2(dx * dx + dy * dy >= radius2 - 1e-7,
                         "Input point is strictly inside a final circumcircle");
            }
        }
    }

    void fourCocircularCornersCoverSquareWithTwoTriangles()
    {
        const auto triangles = geometry::delaunayTriangulation(
            {{0, 0}, {2, 0}, {2, 2}, {0, 2}});
        QCOMPARE(triangles.size(), std::size_t{2});
        double totalArea = 0;
        for (const auto& triangle : triangles) {
            const auto& v = triangle.vertices;
            const double twiceArea = geometry::cross(v[0], v[1], v[2]);
            QVERIFY(twiceArea > 0);
            totalArea += twiceArea / 2;
        }
        QVERIFY(std::abs(totalArea - 4) < 1e-8);
    }

    void interiorPointCreatesFourTrianglesWithoutAreaGaps()
    {
        const auto triangles = geometry::delaunayTriangulation(
            {{0, 0}, {5, 0}, {5, 5}, {0, 5}, {2, 1}});
        QCOMPARE(triangles.size(), std::size_t{4});
        double totalArea = 0;
        for (const auto& triangle : triangles) {
            const auto& v = triangle.vertices;
            totalArea += geometry::cross(v[0], v[1], v[2]) / 2;
        }
        QVERIFY(std::abs(totalArea - 25) < 1e-8);
    }

    void duplicatePointDoesNotCreateExtraTriangles()
    {
        const auto original = geometry::delaunayTriangulation(
            {{0, 0}, {4, 0}, {0, 3}});
        const auto duplicated = geometry::delaunayTriangulation(
            {{0, 0}, {4, 0}, {0, 3}, {4, 0}, {0, 0}});
        QCOMPARE(duplicated.size(), original.size());
        QCOMPARE(duplicated.size(), std::size_t{1});
    }
};

QTEST_APPLESS_MAIN(DelaunayTests)
#include "test_delaunay.moc"
