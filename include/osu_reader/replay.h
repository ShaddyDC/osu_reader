#pragma once

#include <string>
#include <chrono>
#include <vector>
#include <filesystem>
#include <optional>
#include "gamemode.h"

namespace osu{
    using Mods = int;

    struct Replay{
        [[nodiscard]] static std::optional<Replay> from_file(const std::filesystem::path& file_path);
        [[nodiscard]] static std::optional<Replay> from_string(const std::string_view content);

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
        std::chrono::system_clock::time_point time_stamp;
        std::vector<char> replay_compressed;
        std::int64_t score_id;
    };
}
