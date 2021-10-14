#pragma once
#include "osu_reader/beatmap.h"
#include <functional>// TODO: Reconsider include necessity
#include <memory>

namespace osu {
    class Beatmap_parser {
        class Line_provider {
        public:
            virtual std::optional<std::string_view> get_line() = 0;
            virtual ~Line_provider() = default;
        };

    public:
        // disable copies because of iterator
        Beatmap_parser() = default;
        Beatmap_parser(const Beatmap_parser&) = delete;
        const Beatmap_parser& operator==(const Beatmap_parser&) = delete;

        std::optional<Beatmap> from_string(const std::string_view beatmap_content);
        std::optional<Beatmap> from_file(const std::filesystem::path& file_path);

        /// Determines if slider paths are computed
        bool slider_paths = false;

    private:
        enum class Section {
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

        std::optional<Beatmap> parse_impl(Line_provider& line_provider);

        void parse_general(std::string_view line);
        void parse_editor(std::string_view line);
        void parse_metadata(std::string_view line);
        void parse_difficulty(std::string_view line);
        void parse_events(std::string_view line);
        void parse_timingpoints(std::string_view line);
        void parse_hitobject(std::string_view line);
        static bool maybe_parse_utfheader(std::string_view line);
        void parse_line(std::string_view line);
        static Section parse_section(std::string_view line);

        Beatmap beatmap_ = {};
        Section section_ = Section::none;

        using Iterator_t = decltype(beatmap_.timingpoints.cbegin());
        Iterator_t current_timingpoint_;
    };
}// namespace osu
