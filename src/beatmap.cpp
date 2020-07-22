#include "osu_reader/beatmap.h"
#include "osu_reader/beatmap_parser.h"

#include <fstream>

std::optional<osu::Beatmap> osu::Beatmap::from_file(const std::filesystem::path& file_path)
{
	std::ifstream file{ file_path };
	if(!file.is_open()) return std::nullopt;

    const auto line_provider = [&file]() -> std::optional<std::string> {
        std::string line;
	    if(!std::getline(file, line)) return std::nullopt;
        return line;
    };

	return Beatmap_parser{}.parse_impl(line_provider);
}

std::optional<osu::Beatmap> osu::Beatmap::from_string(const std::string_view beatmap_content)
{
    std::size_t last_pos = 0;

    const auto line_provider = [&beatmap_content, &last_pos]() -> std::optional<std::string> {
        if(last_pos == std::string_view::npos || last_pos >= beatmap_content.size()) return std::nullopt;

        const auto pos = beatmap_content.find('\n', last_pos);
        const auto tmp_pos = last_pos;

        last_pos = std::min(pos, beatmap_content.size() - 1) + 1;

        return std::string{ beatmap_content.substr(tmp_pos, pos - tmp_pos) };
    };

	return Beatmap_parser{}.parse_impl(line_provider);
}
