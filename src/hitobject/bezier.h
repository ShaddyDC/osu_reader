#pragma once

#include <osu_reader/vector2.h>
#include <vector>

std::vector<osu::Vector2> approximate_bezier(const std::vector<osu::Vector2>& control_points);
