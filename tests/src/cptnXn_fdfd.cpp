#include <catch2/catch.hpp>
#include <osu_reader/replay.h>
#include "file_string.h"

TEST_CASE("cptnXn_fdfd")
{
    static constexpr const auto filename =
            "res/cptnXn - xi - FREEDOM DiVE [FOUR DIMENSIONS] (2014-05-11) Osu.osr";

    const auto rp_e = GENERATE(
            osu::Replay::from_file(filename),
            osu::Replay::from_string(file_string(filename))
    );

    REQUIRE(rp_e);
    const auto& r = *rp_e;

    REQUIRE(r.mode == Gamemode::osu);
    REQUIRE(r.game_version == 20181231);
    REQUIRE(r.map_hash == "da8aae79c8f3306b5d65ec951874a7fb");
    REQUIRE(r.player_name == "cptnXn");
    REQUIRE(r.replay_hash == "391edbb7774bfa13bd252d5b92c72637");
    REQUIRE(r.count_300 == 1960);
    REQUIRE(r.count_100 == 23);
    REQUIRE(r.count_50 == 0);
    REQUIRE(r.count_geki == 240);
    REQUIRE(r.count_katsu == 11);
    REQUIRE(r.count_miss == 0);
    REQUIRE(r.score == 116845640);
    REQUIRE(r.max_combo == 2384);
    REQUIRE(r.full_combo == false);
    REQUIRE(r.mods == 0);
//    REQUIRE(r.life_bar == "");
//    REQUIRE(r.time_stamp == 0);
    REQUIRE(r.score_id == 1740197996);
}