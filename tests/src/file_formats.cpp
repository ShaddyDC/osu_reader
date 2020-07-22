#include <catch2/catch.hpp>
#include <osu_reader/beatmap.h>

TEST_CASE("No Beatmap")
{
	const auto bm_e = osu::Beatmap::from_file("res/not a file.osu");

	REQUIRE(!bm_e);
}

TEST_CASE("Empty Line Beginning")
{
	const auto bm_e = osu::Beatmap::from_file(
		"res/LamazeP - Koi no Program Hatsudou (feat. Hatsune Miku) (Sonnyc) [Euny's Hard].osu");

	REQUIRE(bm_e.has_value());
	REQUIRE(bm_e->version == 14);
}

TEST_CASE("utf8")
{
	const auto bm_e = osu::Beatmap::from_file("res/Qrispy Joybox feat.mao - Umeyukiyo (LKs) [ampzz's Hard].osu");

	REQUIRE(bm_e.has_value());
	REQUIRE(bm_e->version == 12);
}

TEST_CASE("utf16")
{
	const auto bm_e = osu::Beatmap::from_file("res/An - Necro Fantasia-An remix- (captin1) [Normal].osu");

	REQUIRE(bm_e.has_value());
	REQUIRE(bm_e->version == 14);
}
