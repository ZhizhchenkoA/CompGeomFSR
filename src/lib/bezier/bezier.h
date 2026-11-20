#pragma once

#include <vector>

#include "geometry/point.h"

namespace bezier {

/** @defgroup bezier_api Bezier curves
 *  @brief Parametric curves and de Casteljau construction. */

/** @brief Операции с параметрическими кривыми Безье. */

/** @brief Вычисляет точку кривой Безье в параметре `t` методом де Кастельжо. */
geometry::Point deCasteljau(const std::vector<geometry::Point>& control, double t);

/** @brief Возвращает все промежуточные уровни построения де Кастельжо. */
std::vector<std::vector<geometry::Point>> deCasteljauLevels(
    const std::vector<geometry::Point>& control, double t);

/** @brief Дискретизирует кривую в `count + 1` точках от `t = 0` до `t = 1`. */
std::vector<geometry::Point> sample(const std::vector<geometry::Point>& control,
                                    int count = 160);

/** @brief Строит составную кривую из окон `degree + 1` с общей точкой сегментов. */
std::vector<geometry::Point> composite(const std::vector<geometry::Point>& control,
                                       int degree, int countPerSegment = 70);

} // namespace bezier
