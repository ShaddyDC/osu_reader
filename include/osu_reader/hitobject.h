#pragma once
#include <chrono>
#include <cmath>
#include <optional>
#include <vector>

namespace osu {
    enum class Hitobject_type : std::uint8_t {
        circle = 1,
        slider = 2,
        new_combo = 4,
        spinner = 8,
        mania_holdnote = 128
    };


    struct Point {
        bool operator==(const Point& rhs) const
        {
            return x == rhs.x && y == rhs.y;
        }
        bool operator!=(const Point& rhs) const
        {
            return !(rhs == *this);
        }

        constexpr Point operator+(const Point& rhs) const { return {x + rhs.x, y + rhs.y}; }
        constexpr Point operator-(const Point& rhs) const { return {x - rhs.x, y - rhs.y}; }
        float x, y;
    };
    constexpr Point operator*(const float d, const Point& p) { return {d * p.x, d * p.y}; }
    constexpr float length_squared(const Point& p) { return p.x * p.x + p.y * p.y; }
    constexpr float length(const Point& p) { return std::sqrt(length_squared(p)); }
    constexpr float dot(const Point& a, const Point& b) { return a.x * b.x + a.y * b.y; }
    constexpr Point midpoint(const Point& a, const Point& b) { return 0.5f * (a + b); }

    struct Hitcircle {
        Point pos;
        std::chrono::milliseconds time;
    };

    struct Slider {
        enum class Slider_type : char {
            linear = 'L',
            perfect = 'P',
            bezier = 'B',
            catmull = 'C'
        };

        struct Segment {
            std::vector<Point> points;
            Slider_type type;
        };

        std::chrono::milliseconds time;
        std::chrono::milliseconds duration;
        Slider_type type;
        std::vector<Segment> segments;
        int repeat;
        float length;
    };

    struct Spinner {
        std::chrono::milliseconds start;
        std::chrono::milliseconds end;
    };
}// namespace osu
