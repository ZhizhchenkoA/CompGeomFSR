#pragma once

#include <vector>

#include "geometry/point.h"

namespace geometry {

/** @defgroup geometry_algorithms Geometry algorithms
 *  @brief Platform-independent algorithms used by the workbench. */

/**
 * @brief Выпуклая оболочка множества точек алгоритмом Эндрю.
 * @return Вершины против часовой стрелки без повторения первой точки.
 * Для 0/1/2 различных точек результат содержит 0/1/2 точки.
 * Дубликаты и коллинеарные внутренние точки исключаются.
 * @complexity $O(n \log n)$ по времени, $O(n)$ дополнительной памяти.
 * @invariant Результат ориентирован против часовой стрелки.
 */
std::vector<Point> convexHull(std::vector<Point> points, double eps = 1e-9);

} // namespace geometry
