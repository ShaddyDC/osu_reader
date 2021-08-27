#pragma once

#include <optional>
#include <osu_reader/hitobject.h>
#include <string_view>
#include <vector>

std::optional<osu::Hitcircle> parse_circle(const std::vector<std::string_view>& tokens);
std::optional<osu::Slider> parse_slider(const std::vector<std::string_view>& tokens);
std::optional<osu::Spinner> parse_spinner(const std::vector<std::string_view>& tokens);
