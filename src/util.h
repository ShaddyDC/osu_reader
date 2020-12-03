#pragma once

namespace util {
    inline constexpr char operator"" _ch(const unsigned long long int value)
    {
        return static_cast<char>(value);
    }
}// namespace util
