#pragma once

#include "gamemode.h"
#include <chrono>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace osu {
    enum Mods : uint32_t {
        None = 0,
        NoFail = 1 << 0,
        Easy = 1 << 1,
        TouchDevice = 1 << 2,// Replaces unused NoVideo mod
        Hidden = 1 << 3,
        HardRock = 1 << 4,
        SuddenDeath = 1 << 5,
        DoubleTime = 1 << 6,
        Relax = 1 << 7,
        HalfTime = 1 << 8,
        Nightcore = 1 << 9,// always used with DT : 512 + 64 = 576
        Flashlight = 1 << 10,
        Autoplay = 1 << 11,
        SpunOut = 1 << 12,
        Relax2 = 1 << 13,// Autopilot,
        Perfect = 1 << 14,
        Key4 = 1 << 15,
        Key5 = 1 << 16,
        Key6 = 1 << 17,
        Key7 = 1 << 18,
        Key8 = 1 << 19,
        keyMod = 1015808,// k4+k5+k6+k7+k8
        FadeIn = 1 << 20,
        Random = 1 << 21,
        LastMod = 1 << 22,       // Cinema
        TargetPractice = 1 << 23,// osu!cuttingedge only
        Key9 = 1 << 24,
        Coop = 1 << 25,
        Key1 = 1 << 26,
        Key3 = 1 << 27,
        Key2 = 1 << 28,
        ScoreV2 = 1 << 29,
        Mirror = 1 << 30,
    };

    struct Replay {
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
