#include <catch2/catch.hpp>
#include <osu_reader/beatmap.h>

TEST_CASE("No Beatmap")
{
	const auto bm_e = osu::Beatmap_parser::parse("res/not a file.osu");

	REQUIRE(!bm_e);
	REQUIRE(bm_e.error() == "Couldn't open file");
}
