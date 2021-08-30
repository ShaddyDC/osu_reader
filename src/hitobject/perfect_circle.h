#pragma once

#include <osu_reader/vector2.h>
#include <vector>

[[nodiscard]] std::vector<osu::Vector2> approximate_perfect(const std::vector<osu::Vector2>& control_points);
