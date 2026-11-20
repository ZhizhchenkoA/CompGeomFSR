#pragma once

#include <vector>

#include "geometry/point.h"

namespace geometry {

/** @brief Операция над двумя выпуклыми многоугольниками. */
enum class ConvexPolygonOperation { Intersection, Union, Difference };

/**
 * @brief Набор контуров результата булевой операции.
 * Внешние границы обходятся против часовой стрелки, отверстия — по часовой.
 */
using PolygonSet = std::vector<std::vector<Point>>;

/**
 * @brief Булева операция над двумя выпуклыми многоугольниками (CCW).
 * Поддерживаются непересекающиеся фигуры, касания и несколько компонент.
 */
PolygonSet convexPolygonBoolean(const std::vector<Point>& first,
                                 const std::vector<Point>& second,
                                 ConvexPolygonOperation operation,
                                 double eps = 1e-9);

} // namespace geometry
