#pragma once

namespace geometry {

/** Пространство математической геометрии проекта. */

/**
 * @brief Точка на плоскости в математической системе координат (ось Y вверх).
 *
 * Координаты хранятся как `double`; алгоритмы с произвольной точностью
 * используют отдельный тип `bigint::BigInteger`.
 */
struct Point {
    double x = 0.0;
    double y = 0.0;
};

} // namespace geometry
