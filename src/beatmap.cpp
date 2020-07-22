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
    return std::nullopt;
}
