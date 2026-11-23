#pragma once

#include "geometry/point.h"

namespace geometry {

/**
 * @defgroup geometry_predicates Геометрические предикаты
 * @brief Предикаты знака и принадлежности с учётом допуска.
 * @ingroup geometry_algorithms
 */

/**
 * @brief Векторное произведение `(b - a) x (c - a)`.
 *
 * @f[\operatorname{cross}(a,b,c) = (b_x-a_x)(c_y-a_y) - (b_y-a_y)(c_x-a_x).@f]
 * Положительно, если обход `a -> b -> c` против часовой стрелки.
 * @complexity O(1)
 */
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
 * @post Возвращаемое значение всегда принадлежит множеству `{-1, 0, 1}`.
 * @note Отрицательный `eps` обрабатывается как его абсолютное значение.
 * @warning Для координат, заданных очень длинными десятичными строками,
 * используйте bigint::exactTurn: арифметика `double` может потерять знак.
 * @see cross(), bigint::exactTurn()
 * @educational Знак векторного произведения — основа теста «слева/справа».
 */
int turn(const Point& a, const Point& b, const Point& p, double eps = 1e-9);

/**
 * @brief Проверяет принадлежность точки отрезку с допуском.
 * @param a Первый конец отрезка.
 * @param b Второй конец отрезка.
 * @param p Проверяемая точка.
 * @param eps Евклидово расстояние до ближайшей точки отрезка.
 * @complexity O(1)
 * @par Особый случай
 * Если `a` и `b` совпадают в пределах `eps`, отрезок трактуется как точка.
 */
bool onSegment(const Point& a, const Point& b, const Point& p, double eps = 1e-9);

} // namespace geometry
