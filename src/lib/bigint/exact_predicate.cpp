#include "bigint/exact_predicate.h"

#include <algorithm>
#include <cctype>
#include <limits>
#include <stdexcept>

#include "bigint/bigint.h"

namespace bigint {
namespace {

struct Decimal { BigInteger mantissa; int exponent = 0; };

bool parseDecimal(const std::string& input, Decimal* value, std::string* error)
{
    std::string text;
    for (char ch : input) if (!std::isspace(static_cast<unsigned char>(ch))) text += ch;
    const std::size_t exponentPosition = text.find_first_of("eE");
    if (exponentPosition != std::string::npos && text.find_first_of("eE", exponentPosition + 1) != std::string::npos) {
        *error = "несколько символов экспоненты"; return false;
    }
    std::string number = text.substr(0, exponentPosition);
    int exponent = 0;
    if (exponentPosition != std::string::npos) {
        try {
            std::size_t read = 0;
            exponent = std::stoi(text.substr(exponentPosition + 1), &read);
            if (read != text.size() - exponentPosition - 1) throw std::invalid_argument("tail");
        } catch (...) { *error = "некорректная экспонента"; return false; }
    }
    if (number.empty()) { *error = "пустое число"; return false; }
    int sign = 1;
    if (number.front() == '+' || number.front() == '-') { sign = number.front() == '-' ? -1 : 1; number.erase(0, 1); }
    const std::size_t separator = number.find_first_of(".,");
    if (separator != std::string::npos && number.find_first_of(".,", separator + 1) != std::string::npos) {
        *error = "несколько десятичных разделителей"; return false;
    }
    std::string digits = separator == std::string::npos ? number : number.substr(0, separator) + number.substr(separator + 1);
    if (digits.empty() || !std::all_of(digits.begin(), digits.end(), [](char ch) { return ch >= '0' && ch <= '9'; })) {
        *error = "ожидались цифры"; return false;
    }
    const int fractional = separator == std::string::npos ? 0 : static_cast<int>(number.size() - separator - 1);
    while (digits.size() > 1 && digits.front() == '0') digits.erase(0, 1);
    BigInteger mantissa;
    if (!BigInteger::tryParse((sign < 0 ? "-" : "") + digits, &mantissa)) {
        *error = "некорректное число"; return false;
    }
    if (std::abs(exponent) > 100000 || std::abs(exponent - fractional) > 100000) {
        *error = "слишком большая экспонента"; return false;
    }
    *value = {mantissa, exponent - fractional};
    return true;
}

Decimal subtract(const Decimal& left, const Decimal& right)
{
    const int exponent = std::min(left.exponent, right.exponent);
    BigInteger leftValue = left.mantissa * BigInteger::powerOfTen(left.exponent - exponent);
    BigInteger rightValue = right.mantissa * BigInteger::powerOfTen(right.exponent - exponent);
    return {leftValue - rightValue, exponent};
}

Decimal multiply(const Decimal& left, const Decimal& right)
{
    return {left.mantissa * right.mantissa, left.exponent + right.exponent};
}

} // namespace

bool exactTurn(const std::array<std::string, 6>& coordinates, int* sign, std::string* error)
{
    if (sign == nullptr || error == nullptr) return false;
    std::array<Decimal, 6> values;
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (!parseDecimal(coordinates[i], &values[i], error)) 
        {
            return false;
        }
    }
    const Decimal dxAB = subtract(values[2], values[0]);
    const Decimal dyAB = subtract(values[3], values[1]);
    const Decimal dxAP = subtract(values[4], values[0]);
    const Decimal dyAP = subtract(values[5], values[1]);
    const Decimal first = multiply(dxAB, dyAP);
    const Decimal second = multiply(dyAB, dxAP);
    *sign = subtract(first, second).mantissa.sign();
    return true;
}

} // namespace bigint
