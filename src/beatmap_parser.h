#pragma once
#include "osu_reader/beatmap.h"
#include <optional>
#include <functional>

namespace osu{
	class Beatmap_parser{
	public:
		// disable copies because of iterator
		Beatmap_parser() = default;
		Beatmap_parser(const Beatmap_parser&) = delete;
		const Beatmap_parser& operator==(const Beatmap_parser&) = delete;

		std::optional<Beatmap> parse_impl(const std::function<std::optional<std::string>()>& line_provider);
	private:
		enum class Section{
			general,
			editor,
			metadata,
			difficulty,
			events,
			timingpoints,
			colours,
			hitobjects,
			none
		};

		void parse_general(std::string_view line);
		void parse_editor(std::string_view line);
		void parse_metadata(std::string_view line);
		void parse_difficulty(std::string_view line);
		void parse_events(std::string_view line);
		void parse_timingpoints(std::string_view line);
		void parse_circle(const std::vector<std::string_view>& tokens);
		void parse_slider(const std::vector<std::string_view>& tokens);
		void parse_spinner(const std::vector<std::string_view>& tokens);
		void parse_hitobject(std::string_view line);
		bool maybe_parse_utfheader(std::string_view line);
		void parse_line(std::string_view line);
		static Section parse_section(std::string_view line);

		Beatmap beatmap_ = {};
		Section section_ = Section::none;

		using Iterator_t = decltype(beatmap_.timingpoints.cbegin());
		Iterator_t current_timingpoint_;
	};
}
