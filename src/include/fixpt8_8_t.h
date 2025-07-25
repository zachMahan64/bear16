//
// Created by Zachary Mahan on 7/1/25.
//

#ifndef FIXED_POINT_16_H
#define FIXED_POINT_16_H

#include <bit>
#include <cmath>
#include <cstdint>
#include <type_traits>

struct fixpt8_8_t {
    int16_t val = 0;
    // CONSTRUCTORS
    fixpt8_8_t() = default;
    explicit fixpt8_8_t(uint16_t inp);

    fixpt8_8_t(const fixpt8_8_t &) = default;
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    explicit fixpt8_8_t(T inp) {
        val = static_cast<int16_t>(std::round(inp * 256.0));
    }
    // OPERATORS
    fixpt8_8_t &operator=(const fixpt8_8_t &) = default;

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    fixpt8_8_t &operator=(T inp) {
        val = static_cast<int16_t>(std::round(inp * 256.0));
        return *this;
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    fixpt8_8_t operator*(T rhs) const {
        int32_t scaled_rhs = std::is_floating_point_v<T>
                                 ? static_cast<int32_t>(rhs * 256.0)
                                 : static_cast<int32_t>(rhs) << 8;
        int32_t temp = static_cast<int32_t>(val) * scaled_rhs;
        return fixpt8_8_t(static_cast<int16_t>(temp >> 8));
    }

    fixpt8_8_t operator*(const fixpt8_8_t &rhs) const {
        const int32_t temp = static_cast<int32_t>(val) * rhs.val;
        return fixpt8_8_t(static_cast<int16_t>(temp >> 8));
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    fixpt8_8_t operator/(T rhs) const {
        int32_t scaled_rhs = std::is_floating_point_v<T>
                                 ? static_cast<int32_t>(rhs * 256.0)
                                 : static_cast<int32_t>(rhs) << 8;
        int32_t temp = (static_cast<int32_t>(val) << 8) / scaled_rhs;
        return fixpt8_8_t(static_cast<int16_t>(temp));
    }

    fixpt8_8_t operator/(const fixpt8_8_t &rhs) const {
        const int32_t temp = (static_cast<int32_t>(val) << 8) / rhs.val;
        return fixpt8_8_t(static_cast<int16_t>(temp));
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    fixpt8_8_t operator+(T rhs) const {
        return fixpt8_8_t(val + static_cast<int16_t>(rhs * 256.0));
    }

    fixpt8_8_t operator+(const fixpt8_8_t &rhs) const {
        return fixpt8_8_t(val + rhs.val);
    }

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    fixpt8_8_t operator-(T rhs) const {
        return fixpt8_8_t(val - static_cast<int16_t>(rhs * 256.0));
    }

    fixpt8_8_t operator-(const fixpt8_8_t &rhs) const {
        return fixpt8_8_t(val - rhs.val);
    }

    fixpt8_8_t operator-() const { return fixpt8_8_t(-val); }

    explicit operator float() const { return static_cast<float>(val) / 256.0f; }

    explicit operator uint16_t() const { return val; }

    bool operator==(const fixpt8_8_t &rhs) const { return val == rhs.val; }
    bool operator!=(const fixpt8_8_t &rhs) const { return val != rhs.val; }
    bool operator<(const fixpt8_8_t &rhs) const { return val < rhs.val; }
    bool operator<=(const fixpt8_8_t &rhs) const { return val <= rhs.val; }
    bool operator>(const fixpt8_8_t &rhs) const { return val > rhs.val; }
    bool operator>=(const fixpt8_8_t &rhs) const { return val >= rhs.val; }
};

inline fixpt8_8_t::fixpt8_8_t(const uint16_t inp)
    : val(std::bit_cast<int16_t>(inp)) {}

#endif // FIXED_POINT_16_H
