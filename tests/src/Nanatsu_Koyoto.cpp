#include <catch2/catch.hpp>
#include <osu_reader/beatmap.h>

using namespace std::chrono_literals;

TEST_CASE("Nanatsu Koyoto")
{
	const auto bm_e = osu::Beatmap_parser::parse("res/A.SAKA - Nanatsu Koyoto (ailv) [Extra].osu");

	if(!bm_e.has_value())
		WARN(bm_e.error());

	REQUIRE(bm_e.has_value());

	const auto& bm = bm_e.value();

	CHECK(bm.version == 14);
	CHECK(bm.audio_file == "audio.mp3");
	CHECK(bm.audio_lead_in.count() == (0ms).count());
	CHECK(bm.preview_time.count() == (-1ms).count());
	CHECK(bm.countdown == 1);
	CHECK(bm.sample_set == "Soft");
	CHECK(bm.stack_leniency == 0.7f);
	CHECK(bm.mode == Gamemode::osu);
	CHECK(!bm.letterbox_in_breaks);
	CHECK(bm.widescreen_storyboard);

	// Todo: Bookmarks	
	CHECK(bm.distance_spacing == 0.9f);
	CHECK(bm.beat_divisor == 4);
	CHECK(bm.grid_size == 32);
	CHECK(bm.timeline_zoom == 1.4f);

	CHECK(bm.title == "Nanatsu Koyoto");
	//CHECK(bm.title_unicode == "");
	CHECK(bm.artist == "A.SAKA");
	//CHECK(bm.artist_unicode == "");
	CHECK(bm.creator == "ailv");
	CHECK(bm.difficulty_name == "Extra");
	CHECK(bm.source == "");
	// tags: contain unicode :(
	//CHECK(bm.tags == );	
	CHECK(bm.beatmap_id == 1619968);
	CHECK(bm.beatmap_set_id == 770521);

	CHECK(bm.hp == 5.8f);
	CHECK(bm.cs == 4.f);
	CHECK(bm.od == 8.8f);
	CHECK(bm.ar == 9.4f);
	CHECK(bm.slider_multiplier == 1.8f);
	CHECK(bm.slider_tick_rate == 1.f);

	//CHECK(bm.background == "tochen.jpg");
	// events empty :)

	// Timing Points

	// Hitobjects
}
