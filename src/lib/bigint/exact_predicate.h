#pragma once

#include <array>
#include <string>

namespace bigint {

/** @brief Поля массива идут в порядке `Ax, Ay, Bx, By, Px, Py`. */

/**
 * @brief Точно вычисляет знак `(B - A) x (P - A)` для строковых чисел.
 * @return `true` при успешном разборе входа.
 * @param coordinates Координаты в порядке `Ax, Ay, Bx, By, Px, Py`.
 * @param sign Результат `-1`, `0` или `1`.
 * @param error Описание ошибки разбора.
 */
bool exactTurn(const std::array<std::string, 6>& coordinates, int* sign, std::string* error);

} // namespace bigint
