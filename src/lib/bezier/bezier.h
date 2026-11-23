#pragma once

#include <vector>

#include "geometry/point.h"

namespace bezier {

/** @defgroup bezier_api Кривые Безье
 *  @brief Параметрические кривые и построение де Кастельжо. */

/** @brief Операции с параметрическими кривыми Безье. */

/**
 * @brief Вычисляет точку кривой Безье в параметре `t` методом де Кастельжо.
 *
 * @f[Q_i^{(r)}=(1-t)Q_i^{(r-1)}+tQ_{i+1}^{(r-1)}.@f]
 * @note Значение `t` ограничивается интервалом `[0, 1]`.
 * @par Визуализация
 * Промежуточные уровни построения доступны через deCasteljauLevels().
 * @see deCasteljauLevels()
 */
geometry::Point deCasteljau(const std::vector<geometry::Point>& control, double t);

/** @brief Возвращает все промежуточные уровни построения де Кастельжо. */
std::vector<std::vector<geometry::Point>> deCasteljauLevels(
    const std::vector<geometry::Point>& control, double t);

/** @brief Дискретизирует кривую в `count + 1` точках от `t = 0` до `t = 1`. */
std::vector<geometry::Point> sample(const std::vector<geometry::Point>& control,
                                    int count = 160);

/**
 * @brief Строит составную кривую из окон `degree + 1` с общей точкой сегментов.
 * @pre `degree >= 1` и `countPerSegment >= 1`; при нарушении возвращается пустой набор.
 * @todo Добавить режим сглаживания стыков составной кривой `C¹`.
 */
std::vector<geometry::Point> composite(const std::vector<geometry::Point>& control,
                                       int degree, int countPerSegment = 70);

} // namespace bezier
