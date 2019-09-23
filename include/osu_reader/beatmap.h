#pragma once
#include "beatmap_file.h"
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

		void parse_general(std::string_view line);
		void parse_editor(std::string_view line);
		void parse_metadata(std::string_view line);
		void parse_difficulty(std::string_view line);
		void parse_events(std::string_view line);
		void parse_timing_points(std::string_view line);
		void parse_circle(const std::vector<std::string_view>& tokens);
		void parse_slider(const std::vector<std::string_view>& tokens);
		void parse_spinner(const std::vector<std::string_view>& tokens);
		void parse_hitobject(std::string_view line);
		bool maybe_parse_utfheader(std::string_view line);
		void parse_line(std::string_view line);
		std::optional<Beatmap_file> parse_impl(const std::filesystem::path& file_path);
		static Section parse_section(std::string_view line);

		Beatmap_file beatmap_ = {};
		Section section_      = Section::none;
	};
}
