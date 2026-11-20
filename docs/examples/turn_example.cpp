/**
 * @example turn_example.cpp
 * Минимальный пример использования библиотечного предиката поворота.
 */
#include "geometry/predicates.h"

int main()
{
    const geometry::Point a{0.0, 0.0};
    const geometry::Point b{10.0, 0.0};
    const geometry::Point p{5.0, 3.0};
    return geometry::turn(a, b, p) == 1 ? 0 : 1;
}
