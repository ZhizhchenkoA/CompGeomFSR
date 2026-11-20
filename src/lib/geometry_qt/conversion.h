#pragma once

#include <QPointF>

#include "geometry/point.h"

namespace geometry_qt {

/** @brief Адаптер между экранными координатами Qt и математическими координатами. */

/**
 * @brief Переводит экранную точку в математическую.
 * Экранная ось `Y` направлена вниз; при переводе знак `y` инвертируется.
 */
geometry::Point toGeometry(const QPointF& screen);

/** @brief Обратное преобразование: геометрия -> экран. */
QPointF toScreen(const geometry::Point& geometryPoint);

} // namespace geometry_qt
