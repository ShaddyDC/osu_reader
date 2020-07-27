#include <catch2/catch.hpp>
#include <osu_reader/beatmap_util.h>

TEST_CASE("AR")
{
	REQUIRE(osu::ar_to_ms(0) == 1800);
	REQUIRE(osu::ar_to_ms(1) == 1680);
	REQUIRE(osu::ar_to_ms(4.5) == 1260);
	REQUIRE(osu::ar_to_ms(5) == 1200);
	REQUIRE(osu::ar_to_ms(6) == 1050);
	REQUIRE(osu::ar_to_ms(10) == 450);
	REQUIRE(static_cast<int>(osu::ar_to_ms(10.33)) == 400);
	REQUIRE(osu::ar_to_ms(11) == 300);
}

TEST_CASE("CS")
{
    // Someone send me test data
}

TEST_CASE("OD")
{
	REQUIRE(osu::od_to_ms300(0) == 79.5);
	REQUIRE(osu::od_to_ms100(0) == 139.5);
	REQUIRE(osu::od_to_ms50(0) == 199.5);

	REQUIRE(osu::od_to_ms300(5) == 49.5);
	REQUIRE(osu::od_to_ms100(5) == 99.5);
	REQUIRE(osu::od_to_ms50(5) == 149.5);

	REQUIRE(osu::od_to_ms300(10) == 19.5);
	REQUIRE(osu::od_to_ms100(10) == 59.5);
	REQUIRE(osu::od_to_ms50(10) == 99.5);
}

