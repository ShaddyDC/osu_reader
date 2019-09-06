#include <catch2/catch.hpp>
#include <osu_reader/beatmap.h>

using namespace std::chrono_literals;

TEST_CASE("Kakoi-naki")
{
	const auto bm_e = osu::Beatmap_parser::parse(
		"res/Buta-Otome - Kakoi-naki Yo wa Ichigo no Tsukikage (BarkingMadDog) [this map is so bad cuz overmapping].osu");

	if(!bm_e.has_value())
		WARN(bm_e.error());

	REQUIRE(bm_e.has_value());

	const auto& bm = bm_e.value();

	CHECK(bm.version == 14);
	CHECK(bm.audio_file == "Buta-Otome - Kakoi-naki Yo wa Ichigo no Tsukikage.mp3");
	CHECK(bm.audio_lead_in.count() == (0ms).count());
	CHECK(bm.preview_time.count() == (56710ms).count());
	CHECK(bm.countdown == 0);
	CHECK(bm.sample_set == "Soft");
	CHECK(bm.stack_leniency == 0.7f);
	CHECK(bm.mode == Gamemode::osu);
	CHECK(bm.letterbox_in_breaks);
	CHECK(!bm.widescreen_storyboard);

	CHECK(bm.distance_spacing == 0.6f);
	CHECK(bm.beat_divisor == 4);
	CHECK(bm.grid_size == 4);
	CHECK(bm.timeline_zoom == 1);

	CHECK(bm.title == "Kakoi-naki Yo wa Ichigo no Tsukikage");
	//CHECK(bm.title_unicode == "");
	CHECK(bm.artist == "Buta-Otome");
	//CHECK(bm.artist_unicode == "");
	CHECK(bm.creator == "BarkingMadDog");
	CHECK(bm.difficulty_name == "this map is so bad cuz overmapping");
	//CHECK(bm.source == "");
	// tags: contain unicode :(
	//CHECK(bm.tags == );	
	CHECK(bm.beatmap_id == 1747613);
	CHECK(bm.beatmap_set_id == 834245);

	CHECK(bm.hp == 7.f);
	CHECK(bm.cs == 4.2f);
	CHECK(bm.od == 8.5f);
	CHECK(bm.ar == 9.3f);
	CHECK(bm.slider_multiplier == 1.84f);
	CHECK(bm.slider_tick_rate == 1.f);

	CHECK(bm.background == "tochen.jpg");

	// Timing Points
	CHECK(bm.timing_points.size() == 39);
	if(bm.timing_points.size() > 1){
		const auto tm = bm.timing_points[1];
		CHECK(tm.time.count() == (4221ms).count());
		CHECK(tm.beat_duration.count() == std::chrono::duration_cast<std::chrono::microseconds>(840.335ms).count());
		CHECK(tm.meter == 4);
		CHECK(tm.sample_set == 2);
		CHECK(tm.sample_index == 1);
		CHECK(tm.sample_volume == 34);
		CHECK(!tm.kiai);
	}

	// Hitobjects
}
