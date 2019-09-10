#pragma once
#include "beatmap_file.h"
#include <fstream>
#include <optional>

namespace osu{
	class Beatmap_parser{
	public:
		static std::optional<Beatmap_file> parse(const std::filesystem::path& file);

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
		bool maybe_parse_utfheader();
		void parse_line(const std::string_view line);
		std::optional<Beatmap_file> parse_impl();
		static Section parse_section(std::string_view line);

		std::ifstream file_;
		Beatmap_file beatmap_ = {};
		Section section_      = Section::none;
	};
}
