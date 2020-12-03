#pragma once
#include <cstdint>

namespace osu {
    enum class Gamemode : std::uint8_t {
        osu = 0,
        taiko = 1,
        ctb = 2,
        mania = 3
    };
}
