#pragma once

#include <string_view>
#include <chrono>
#include "osu_reader/beatmap.h"
#include "string_stuff.h"
#include <filesystem>
#include <vector>

template<typename Type>
void parse_value(std::string_view value_string, Type& value)
{
	std::from_chars(value_string.data(), value_string.data() + value_string.length(), value);
}

// TODO: remove these when floating point from_chars is more widely supported
template<>
inline void parse_value<>(std::string_view value_string, float& value)
{
	value = std::stof(std::string{ value_string });
}

template<>
inline void parse_value<>(std::string_view value_string, double& value)
{
	value = std::stod(std::string{ value_string });
}

template<>
inline void parse_value<>(std::string_view value_string, bool& value)
{
	if(!value_string.empty()){
		if(value_string[0] == '1') value = true;
		else if(value_string[0] == '0') value = false;
	}
}

template<>
inline void parse_value<>(std::string_view value_string, std::chrono::milliseconds& value)
{
	auto v = 0;
	parse_value(value_string, v);
	value = std::chrono::milliseconds{ v };
}

template<>
inline void parse_value<>(std::string_view value_string, Gamemode& value)
{
	auto v = 0;
	parse_value(value_string, v);
	value = static_cast<Gamemode>(value);
}

template<>
inline void parse_value<>(std::string_view value_string, std::string& value)
{
	value = value_string;
}

template<>
inline void parse_value<>(std::string_view value_string, std::filesystem::path& value)
{
	std::string s;
	parse_value(value_string, s);
	value = s;
}

template<>
inline void parse_value<>(std::string_view value_string, std::vector<std::string>& value)
{
	const auto tokens = split(value_string, ' ');
	std::transform(tokens.cbegin(), tokens.cend(), std::back_inserter(value),
	               [](const auto e)
	               {
		               return std::string{ e };
	               });
}

template<>
inline void parse_value<>(std::string_view value_string, std::vector<std::chrono::milliseconds>& value)
{
	const auto tokens = split(value_string, ' ');
	std::transform(tokens.cbegin(), tokens.cend(), std::back_inserter(value),
	               [](const std::string_view s)
	               {
		               const auto trimmed = ltrim_view(s);
		               int v;
		               std::from_chars(trimmed.data(), trimmed.data() + trimmed.length(), v);
		               return std::chrono::milliseconds{ v };
	               });
}

template<typename Type>
Type parse_value(std::string_view value_string)
{
	Type value{};
	parse_value(value_string, value);
	return value;
}
