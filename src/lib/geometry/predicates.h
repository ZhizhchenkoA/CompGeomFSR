#pragma once

#include "geometry/point.h"

namespace geometry {

/**
 * @defgroup geometry_predicates Geometric predicates
 * @brief Exact-sign and tolerance-aware predicates used by higher algorithms.
 * @ingroup geometry_algorithms
 */

/** @brief Векторное произведение `(b - a) x (c - a)`.
 *  Положительно, если обход `a -> b -> c` против часовой стрелки. */
/** @ingroup geometry_predicates */
double cross(const Point& a, const Point& b, const Point& c);

/**
 * @brief Предикат поворота.
 * @return `1`, если `p` слева от `ab`; `-1`, если справа; `0`, если на прямой.
 * @param a Начало направленного отрезка.
 * @param b Конец направленного отрезка.
 * @param p Проверяемая точка.
 * @param eps Допуск, при котором значение считается нулём.
 * @complexity O(1)
 */
int turn(const Point& a, const Point& b, const Point& p, double eps = 1e-9);

/**
 * @brief Проверяет принадлежность точки отрезку с допуском.
 * @param a Первый конец отрезка.
 * @param b Второй конец отрезка.
 * @param p Проверяемая точка.
 * @param eps Евклидово расстояние до ближайшей точки отрезка.
 * @complexity O(1)
 */
bool onSegment(const Point& a, const Point& b, const Point& p, double eps = 1e-9);

} // namespace geometry
