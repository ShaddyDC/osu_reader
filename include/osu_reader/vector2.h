#pragma once

#include <cmath>

namespace osu {
    struct Vector2 {
        bool operator==(const Vector2& rhs) const
        {
            return x == rhs.x && y == rhs.y;
        }
        bool operator!=(const Vector2& rhs) const
        {
            return !(rhs == *this);
        }

        constexpr Vector2 operator+(const Vector2& rhs) const { return {x + rhs.x, y + rhs.y}; }
        constexpr Vector2 operator-(const Vector2& rhs) const { return {x - rhs.x, y - rhs.y}; }
        float x, y;
    };
    constexpr inline Vector2 operator*(const float d, const Vector2& p) { return {d * p.x, d * p.y}; }
    constexpr inline float length_squared(const Vector2& p) { return p.x * p.x + p.y * p.y; }
    constexpr inline float length(const Vector2& p) { return std::sqrt(length_squared(p)); }
    constexpr inline float dot(const Vector2& a, const Vector2& b) { return a.x * b.x + a.y * b.y; }
    constexpr inline Vector2 normal(const Vector2& p) { return 1 / length(p) * p; }
    constexpr inline Vector2 midpoint(const Vector2& a, const Vector2& b) { return 0.5f * (a + b); }
}// namespace osu
