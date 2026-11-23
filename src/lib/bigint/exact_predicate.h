#pragma once

#include <array>
#include <string>

namespace bigint {

/** @brief Поля массива идут в порядке `Ax, Ay, Bx, By, Px, Py`. */

/**
 * @brief Точно вычисляет знак `(B - A) x (P - A)` для строковых чисел.
 *
 * @anchor exact_turn_pipeline
 * @par Конвейер точного вычисления
 * @dot
 * digraph exact_turn {
 *   rankdir=LR;
 *   node [shape=box, style=rounded];
 *   input [label="Строковые координаты"];
 *   parse [label="Мантисса + степень 10"];
 *   product [label="(Bx-Ax)(Py-Ay) - (By-Ay)(Px-Ax)"];
 *   sign [label="Знак BigInteger"];
 *   input -> parse -> product -> sign;
 * }
 * @enddot
 *
 * @f[\operatorname{sign}\big((B-A) \times (P-A)\big) \in \{-1,0,1\}.@f]
 * @pre Указатели `sign` и `error` не равны `nullptr`.
 * @post При успехе `*sign` имеет значение `-1`, `0` или `1`.
 * @warning Экспоненты с модулем больше `100000` отклоняются, чтобы ограничить
 * объём памяти, требуемый для выравнивания десятичных степеней.
 * @see geometry::turn()
 * @since 1.0
 * @return `true` при успешном разборе входа.
 * @param coordinates Координаты в порядке `Ax, Ay, Bx, By, Px, Py`.
 * @param sign Результат `-1`, `0` или `1`.
 * @param error Описание ошибки разбора.
 */
bool exactTurn(const std::array<std::string, 6>& coordinates, int* sign, std::string* error);

} // namespace bigint
