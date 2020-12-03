#include "file_string.h"
#include <catch2/catch.hpp>
#include <osu_reader/beatmap.h>

TEST_CASE("No Beatmap")
{
    const auto bm_e = osu::Beatmap::from_file("res/not a file.osu");

    REQUIRE(!bm_e);
}

TEST_CASE("Empty Line Beginning")
{
    static constexpr const char* filename = "res/LamazeP - Koi no Program Hatsudou (feat. Hatsune Miku) (Sonnyc) [Euny's Hard].osu";
    const auto bm_e = GENERATE(
            osu::Beatmap::from_file(filename),
            osu::Beatmap::from_string(file_string(filename)));

    REQUIRE(bm_e.has_value());
    REQUIRE(bm_e->version == 14);
}

TEST_CASE("utf8")
{
    static constexpr const char* filename = "res/Qrispy Joybox feat.mao - Umeyukiyo (LKs) [ampzz's Hard].osu";
    const auto bm_e = GENERATE(
            osu::Beatmap::from_file(filename),
            osu::Beatmap::from_string(file_string(filename)));

    REQUIRE(bm_e.has_value());
    REQUIRE(bm_e->version == 12);
}

TEST_CASE("utf16")
{
    static constexpr const char* filename = "res/An - Necro Fantasia-An remix- (captin1) [Normal].osu";
    const auto bm_e = GENERATE(
            osu::Beatmap::from_file(filename),
            osu::Beatmap::from_string(file_string(filename)));

    REQUIRE(bm_e.has_value());
    REQUIRE(bm_e->version == 14);
}
