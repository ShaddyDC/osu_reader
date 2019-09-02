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

	CHECK(bm.bookmarks.size() == 1);
	if(!bm.bookmarks.empty()) CHECK(bm.bookmarks[0].count() == (53600ms).count());
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

	CHECK(bm.background == "Loundraw.full.214746799.jpg");
	CHECK(bm.breaks.size() == 1);
	if(!bm.breaks.empty()){
		const auto brk = bm.breaks[0];
		CHECK(brk.first.count() == (75133ms).count());
		CHECK(brk.second.count() == (85060ms).count());
	}

	CHECK(bm.timing_points.size() == 11);
	if(bm.timing_points.size() > 1){
		const auto tm = bm.timing_points[1];
		CHECK(tm.time.count() == (21600ms).count());
		CHECK(tm.beat_duration.count() == std::chrono::duration_cast<std::chrono::microseconds>(277.777777777777ms).count());
		CHECK(tm.meter == 4);
		CHECK(tm.sample_set == 2);
		CHECK(tm.sample_index == 1);
		CHECK(tm.sample_volume == 90);
		CHECK(tm.kiai);
	}
	// Timing Points

	// Hitobjects
}
