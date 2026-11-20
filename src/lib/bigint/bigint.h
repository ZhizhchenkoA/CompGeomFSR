#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace bigint {

/** @defgroup bigint_api Arbitrary-precision arithmetic
 *  @brief Signed integers and exact decimal predicates. */

/** @brief Библиотека точной арифметики для задачи с длинными координатами. */

/** @brief Знаковое целое произвольной длины в базе `10^9`. */
class BigInteger {
public:
    /** @brief Создаёт нулевое число. */
    BigInteger() = default;
    /** @brief Создаёт число из значения машинного типа. */
    BigInteger(long long value);

    /** @brief Разбирает десятичную строку со знаком; при ошибке возвращает `false`. */
    static bool tryParse(const std::string& text, BigInteger* result);
    /** @brief Возвращает десятичное представление без ведущих нулей. */
    std::string toString() const;
    /** @brief Возвращает знак: `-1`, `0` или `1`. */
    int sign() const;
    /** @brief Проверяет, равно ли число нулю. */
    bool isZero() const;

    /** @brief Унарное изменение знака. */
    BigInteger operator-() const;
    /** @brief Сложение, вычитание и умножение произвольных целых. */
    BigInteger& operator+=(const BigInteger& other);
    BigInteger& operator-=(const BigInteger& other);
    BigInteger& operator*=(const BigInteger& other);
    friend BigInteger operator+(BigInteger left, const BigInteger& right) { return left += right; }
    friend BigInteger operator-(BigInteger left, const BigInteger& right) { return left -= right; }
    friend BigInteger operator*(BigInteger left, const BigInteger& right) { return left *= right; }

    static BigInteger powerOfTen(std::size_t exponent);

private:
    static constexpr std::uint32_t kBase = 1'000'000'000;
    int m_sign = 0;
    std::vector<std::uint32_t> m_digits;

    void normalize();
    static int compareAbs(const BigInteger& left, const BigInteger& right);
    void addAbs(const BigInteger& other);
    void subtractAbs(const BigInteger& other); // |*this| >= |other|
};

} // namespace bigint
