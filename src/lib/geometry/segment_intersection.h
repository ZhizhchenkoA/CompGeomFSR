#pragma once

#include "geometry/point.h"

namespace geometry {

/** @brief Тип результата пересечения двух замкнутых отрезков. */
enum class SegmentIntersectionKind {
    None,
    Point,
    Overlap,
};

/**
 * @brief Результат пересечения замкнутых отрезков.
 * Для `Point` используется `first`, для `Overlap` — границы `first--second`.
 */
struct SegmentIntersection {
    SegmentIntersectionKind kind = SegmentIntersectionKind::None;
    Point first{};
    Point second{};
};

/**
 * @brief Находит пересечение замкнутых отрезков `AB` и `CD`.
 * @param a Первый конец первого отрезка.
 * @param b Второй конец первого отрезка.
 * @param c Первый конец второго отрезка.
 * @param d Второй конец второго отрезка.
 * @param eps Допуск в единицах координат; учитывает касание концом.
 * @complexity O(1)
 */
SegmentIntersection intersectSegments(const Point& a, const Point& b,
                                      const Point& c, const Point& d,
                                      double eps = 1e-9);

} // namespace geometry
