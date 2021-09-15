#pragma once

#include <osu_reader/hitobject.h>

namespace osu {
    [[nodiscard]] std::vector<osu::Vector2> sliderpath(const osu::Slider& slider);
    [[nodiscard]] std::vector<float> pathlengths(const std::vector<osu::Vector2>& points);
    void fix_slider_length(osu::Slider& slider);
}// namespace osu
