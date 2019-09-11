#include <catch2/catch.hpp>
#include <osu_reader/beatmap.h>
#include <type_traits>

TEST_CASE("Parser Properties")
{
	STATIC_REQUIRE_FALSE(std::is_copy_assignable_v<osu::Beatmap_parser>);
	STATIC_REQUIRE_FALSE(std::is_move_assignable_v<osu::Beatmap_parser>);
	STATIC_REQUIRE_FALSE(std::is_copy_constructible_v<osu::Beatmap_parser>);
	STATIC_REQUIRE_FALSE(std::is_move_constructible_v<osu::Beatmap_parser>);
}