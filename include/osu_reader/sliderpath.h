#pragma once

#include <osu_reader/hitobject.h>

namespace osu {
    [[nodiscard]] std::vector<osu::Vector2> sliderpath(const osu::Slider& slider);
}// namespace osu
