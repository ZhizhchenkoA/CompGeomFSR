#pragma once

#include <array>
#include <vector>

#include "geometry/point.h"

namespace geometry {

/** @brief Окружность, описанная около треугольника. */
struct Circumcircle {
    Point center{};
    double radius = 0.0;
};

/** @brief Треугольник Делоне и его описанная окружность. */
struct DelaunayTriangle {
    std::array<Point, 3> vertices{};
    Circumcircle circumcircle{};
};

/**
 * @brief Триангуляция Делоне алгоритмом Bowyer--Watson.
 * Дубликаты исключаются; для менее чем трёх неколлинеарных точек
 * возвращается пустой список. Точки на окружности допускаются.
 * @complexity В среднем O(n log n), в худшем случае зависит от числа треугольников.
 */
std::vector<DelaunayTriangle> delaunayTriangulation(std::vector<Point> points,
                                                     double eps = 1e-9);

} // namespace geometry
