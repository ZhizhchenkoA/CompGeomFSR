#include "bigint/bigint.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace bigint {

BigInteger::BigInteger(long long value)
{
    if (value == 0) return;
    m_sign = value < 0 ? -1 : 1;
    std::uint64_t magnitude = value < 0
        ? static_cast<std::uint64_t>(-(value + 1)) + 1
        : static_cast<std::uint64_t>(value);
    while (magnitude != 0) {
        m_digits.push_back(static_cast<std::uint32_t>(magnitude % kBase));
        magnitude /= kBase;
    }
}

bool BigInteger::tryParse(const std::string& text, BigInteger* result)
{
    if (text.empty() || result == nullptr) return false;
    std::size_t begin = 0;
    int sign = 1;
    if (text[begin] == '+' || text[begin] == '-') {
        sign = text[begin++] == '-' ? -1 : 1;
    }
    if (begin == text.size()) return false;
    for (std::size_t i = begin; i < text.size(); ++i) {
        if (text[i] < '0' || text[i] > '9') return false;
    }
    BigInteger value;
    for (std::size_t end = text.size(); end > begin;) {
        const std::size_t start = end >= begin + 9 ? end - 9 : begin;
        value.m_digits.push_back(static_cast<std::uint32_t>(std::stoul(text.substr(start, end - start))));
        end = start;
    }
    value.m_sign = sign;
    value.normalize();
    *result = std::move(value);
    return true;
}

std::string BigInteger::toString() const
{
    if (m_sign == 0) return "0";
    std::ostringstream stream;
    if (m_sign < 0) stream << '-';
    stream << m_digits.back();
    for (std::size_t i = m_digits.size() - 1; i > 0; --i) {
        stream << std::setw(9) << std::setfill('0') << m_digits[i - 1];
    }
    return stream.str();
}

int BigInteger::sign() const { return m_sign; }
bool BigInteger::isZero() const { return m_sign == 0; }

BigInteger BigInteger::operator-() const
{
    BigInteger result = *this;
    result.m_sign = -result.m_sign;
    return result;
}

BigInteger& BigInteger::operator+=(const BigInteger& other)
{
    if (other.m_sign == 0) return *this;
    if (m_sign == 0) { *this = other; return *this; }
    if (m_sign == other.m_sign) { addAbs(other); return *this; }
    const int comparison = compareAbs(*this, other);
    if (comparison == 0) { m_digits.clear(); m_sign = 0; return *this; }
    if (comparison > 0) { subtractAbs(other); return *this; }
    BigInteger result = other;
    result.subtractAbs(*this);
    *this = std::move(result);
    return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& other) { return *this += -other; }

BigInteger& BigInteger::operator*=(const BigInteger& other)
{
    if (m_sign == 0 || other.m_sign == 0) { m_digits.clear(); m_sign = 0; return *this; }
    std::vector<std::uint64_t> product(m_digits.size() + other.m_digits.size());
    for (std::size_t i = 0; i < m_digits.size(); ++i) {
        std::uint64_t carry = 0;
        for (std::size_t j = 0; j < other.m_digits.size(); ++j) {
            const std::uint64_t value = product[i + j] +
                static_cast<std::uint64_t>(m_digits[i]) * other.m_digits[j] + carry;
            product[i + j] = value % kBase;
            carry = value / kBase;
        }
        product[i + other.m_digits.size()] += carry;
    }
    m_digits.assign(product.begin(), product.end());
    std::uint64_t carry = 0;
    for (std::size_t i = 0; i < m_digits.size(); ++i) {
        const std::uint64_t value = m_digits[i] + carry;
        m_digits[i] = static_cast<std::uint32_t>(value % kBase);
        carry = value / kBase;
    }
    if (carry) m_digits.push_back(static_cast<std::uint32_t>(carry));
    m_sign *= other.m_sign;
    normalize();
    return *this;
}

BigInteger BigInteger::powerOfTen(std::size_t exponent)
{
    std::string value("1");
    value.append(exponent, '0');
    BigInteger result;
    tryParse(value, &result);
    return result;
}

void BigInteger::normalize()
{
    while (!m_digits.empty() && m_digits.back() == 0) m_digits.pop_back();
    if (m_digits.empty()) m_sign = 0;
}

int BigInteger::compareAbs(const BigInteger& left, const BigInteger& right)
{
    if (left.m_digits.size() != right.m_digits.size())
        return left.m_digits.size() < right.m_digits.size() ? -1 : 1;
    for (std::size_t i = left.m_digits.size(); i > 0; --i) {
        if (left.m_digits[i - 1] != right.m_digits[i - 1])
            return left.m_digits[i - 1] < right.m_digits[i - 1] ? -1 : 1;
    }
    return 0;
}

void BigInteger::addAbs(const BigInteger& other)
{
    std::uint64_t carry = 0;
    const std::size_t count = std::max(m_digits.size(), other.m_digits.size());
    m_digits.resize(count);
    for (std::size_t i = 0; i < count; ++i) {
        const std::uint64_t value = m_digits[i] + (i < other.m_digits.size() ? other.m_digits[i] : 0) + carry;
        m_digits[i] = static_cast<std::uint32_t>(value % kBase);
        carry = value / kBase;
    }
    if (carry) m_digits.push_back(static_cast<std::uint32_t>(carry));
}

void BigInteger::subtractAbs(const BigInteger& other)
{
    std::int64_t borrow = 0;
    for (std::size_t i = 0; i < m_digits.size(); ++i) {
        std::int64_t value = static_cast<std::int64_t>(m_digits[i]) -
            (i < other.m_digits.size() ? other.m_digits[i] : 0) - borrow;
        if (value < 0) { value += kBase; borrow = 1; } else borrow = 0;
        m_digits[i] = static_cast<std::uint32_t>(value);
    }
    normalize();
}

} // namespace bigint
