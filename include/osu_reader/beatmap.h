#pragma once
#include "beatmap_file.h"
#include <fstream>
#include <tl/expected.hpp>

namespace osu{
	class Beatmap_parser{
	public:
		static tl::expected<Beatmap_file, std::string> parse(const std::filesystem::path& file);

	private:
		enum class Section{
			general,
			editor,
			metadata,
			difficulty,
			events,
			timing_points,
			colours,
			hitobjects,
			none
		};

		explicit Beatmap_parser(const std::filesystem::path& file);
		bool parse_general(std::string_view line);
		bool maybe_parse_bookmarks(std::string_view line);
		bool parse_editor(std::string_view line);
		bool parse_metadata(std::string_view line);
		bool parse_difficulty(std::string_view line);
		bool parse_events(std::string_view line);
		bool parse_timing_points(std::string_view line);
		bool parse_hitobjects(std::string_view line);
		tl::expected<Beatmap_file, std::string> parse_impl();
		static Section parse_section(std::string_view line);

		std::ifstream file_;
		Beatmap_file beatmap_ = {};
		Section section_      = Section::none;
	};
}
