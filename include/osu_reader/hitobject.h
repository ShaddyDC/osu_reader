#pragma once
#include <chrono>
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
        float x, y;
    };

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
