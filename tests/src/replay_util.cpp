#include <catch2/catch.hpp>
#include <osu_reader/replay.h>

TEST_CASE("Mods logic")
{
    REQUIRE(static_cast<int>(osu::Mods::None) == 0);
    REQUIRE(static_cast<int>(osu::Mods::Hidden | osu::Mods::HardRock) == 24);
    REQUIRE(osu::has_mods(osu::Mods::Hidden | osu::Mods::HardRock, osu::Mods::Hidden));
    REQUIRE(osu::has_mods(osu::Mods::Hidden | osu::Mods::HardRock, osu::Mods::Hidden | osu::Mods::HardRock));
    REQUIRE(osu::has_mods(osu::Mods::Hidden | osu::Mods::HardRock, osu::Mods::None));
    REQUIRE(osu::has_mods(osu::Mods::None, osu::Mods::None));
    REQUIRE_FALSE(osu::has_mods(osu::Mods::None, osu::Mods::Hidden));
    REQUIRE_FALSE(osu::has_mods(osu::Mods::Hidden, osu::Mods::Hidden | osu::Mods::HardRock));
}
