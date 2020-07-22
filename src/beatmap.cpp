#include "osu_reader/beatmap.h"
#include "osu_reader/beatmap_parser.h"

std::optional<osu::Beatmap> osu::Beatmap::from_file(const std::filesystem::path& file)
{
	return Beatmap_parser{}.parse_impl(file);
}
