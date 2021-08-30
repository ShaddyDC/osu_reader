#pragma once

#include "gamemode.h"
#include <chrono>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace osu {
    using Mods = int;

    struct Replay {
        [[nodiscard]] static std::optional<Replay> from_file(const std::filesystem::path& file_path, bool parse_frames = false);
        [[nodiscard]] static std::optional<Replay> from_string(std::string_view content, bool parse_frames = false);

        struct Replay_frame {
            std::chrono::milliseconds time;
            float x, y;
            int state;
        };

        Gamemode mode;
        int game_version;
        std::string map_hash;
        std::string player_name;
        std::string replay_hash;
        std::uint16_t count_300;
        std::uint16_t count_100;
        std::uint16_t count_50;
        std::uint16_t count_geki;
        std::uint16_t count_katsu;
        std::uint16_t count_miss;
        int score;
        std::uint16_t max_combo;
        bool full_combo;
        Mods mods;
        std::string life_bar;
        std::chrono::time_point<std::chrono::nanoseconds> time_stamp;
        std::vector<char> replay_compressed;
        std::optional<std::vector<Replay_frame>> frames;
        std::int64_t score_id;
    };
}// namespace osu
