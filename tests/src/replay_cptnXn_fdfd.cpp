#include "file_string.h"
#include <catch2/catch.hpp>
#include <osu_reader/replay_reader.h>

#ifdef ENABLE_LZMA
constexpr const bool lzma_enabled = true;
#else
constexpr const bool lzma_enabled = false;
#endif

TEST_CASE("cptnXn_fdfd")
{
        static constexpr const auto filename =
                "res/cptnXn - xi - FREEDOM DiVE [FOUR DIMENSIONS] (2014-05-11) Osu.osr";

        // Captured in GENERATE_REF, has to outlive scope
        static auto parser = osu::Replay_reader{};
        parser.parse_frames = lzma_enabled;

        const auto rp_e = GENERATE_REF(
                parser.from_file(filename),
                parser.from_string(file_string(filename)));

        REQUIRE(rp_e);
        const auto& r = *rp_e;

        CHECK(r.mode == osu::Gamemode::osu);
        CHECK(r.game_version == 20181231);
        CHECK(r.map_hash == "da8aae79c8f3306b5d65ec951874a7fb");
        CHECK(r.player_name == "cptnXn");
        CHECK(r.replay_hash == "391edbb7774bfa13bd252d5b92c72637");
        CHECK(r.count_300 == 1960);
        CHECK(r.count_100 == 23);
        CHECK(r.count_50 == 0);
        CHECK(r.count_geki == 240);
        CHECK(r.count_katsu == 11);
        CHECK(r.count_miss == 0);
        CHECK(r.score == 116845640);
        CHECK(r.max_combo == 2384);
        CHECK(r.full_combo == false);
        CHECK(static_cast<int>(r.mods) == 0);
        //    REQUIRE(r.life_bar == "");
        //    REQUIRE(r.time_stamp == 0);
        CHECK(r.score_id == 1740197996);

        if(lzma_enabled) {
            CHECKED_IF(r.frames)
            {
                const auto frames = *r.frames;
                CHECKED_IF(frames.size() == 21188)
                {
                    CHECK(frames[0].time.count() == 0);
                    CHECK(frames[0].x == 256);
                    CHECK(frames[0].y == -500);
                    CHECK(frames[0].state == 0);
                    CHECK(frames[1].time == std::chrono::milliseconds{-1});
                    CHECK(frames[1].x == 256);
                    CHECK(frames[1].y == -500);
                    CHECK(frames[1].state == 0);
                    CHECK(frames[3].time == std::chrono::milliseconds{-11});
                    CHECK(frames[3].x == 233.0667f);
                    CHECK(frames[3].y == 138.6667f);
                    CHECK(frames[3].state == 10);
                    CHECK(frames.back().x == 0);
                    CHECK(frames.back().y == 0);
                    CHECK(frames.back().state == 19467063);
                    CHECK((frames.end() - 2)->time == std::chrono::milliseconds{264331});
                }
            }
        } else {
            WARN("Skipping LZMA data parsing and tests");
            CHECK(r.frames->empty());
        }
}

TEST_CASE("replay without frames")
{
    static constexpr const auto filename =
            "res/cptnXn - xi - FREEDOM DiVE [FOUR DIMENSIONS] (2014-05-11) Osu.osr";

    // Captured in GENERATE_REF, has to outlive scope
    static auto parser = osu::Replay_reader{};
    parser.parse_frames = false;

    const auto rp_e = GENERATE_REF(
            parser.from_file(filename),
            parser.from_string(file_string(filename)));

    REQUIRE(!rp_e->frames);
}
