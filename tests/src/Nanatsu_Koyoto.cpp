#include "file_string.h"
#include <catch2/catch.hpp>
#include <osu_reader/beatmap.h>

using namespace std::chrono_literals;

TEST_CASE("Nanatsu Koyoto")
{
    static constexpr const char* filename = "res/A.SAKA - Nanatsu Koyoto (ailv) [Extra].osu";
    const auto bm_e = GENERATE(
            osu::Beatmap::from_file(filename),
            osu::Beatmap::from_string(file_string(filename)));

    REQUIRE(bm_e.has_value());

    const auto& bm = *bm_e;

    CHECK(bm.version == 14);
    CHECK(bm.audio_file == "audio.mp3");
    CHECK(bm.audio_lead_in.count() == (0ms).count());
    CHECK(bm.preview_time.count() == (-1ms).count());
    CHECK(bm.countdown == 1);
    CHECK(bm.sample_set == "Soft");
    CHECK(bm.stack_leniency == 0.7f);
    CHECK(bm.mode == osu::Gamemode::osu);
    CHECK(!bm.letterbox_in_breaks);
    CHECK(bm.widescreen_storyboard);

    CHECK(bm.bookmarks.size() == 1);
    CHECKED_IF(!bm.bookmarks.empty())
    CHECK(bm.bookmarks[0].count() == (53600ms).count());
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
    CHECK(bm.source.empty());
    // tags: contain unicode :(
    CHECK(bm.tags.size() == 2);
    CHECK(bm.beatmap_id == 1619968);
    CHECK(bm.beatmap_set_id == 770521);

    CHECK(bm.hp == 5.8f);
    CHECK(bm.cs == 4.f);
    CHECK(bm.od == 8.8f);
    CHECK(bm.ar == 9.4f);
    CHECK(bm.slider_multiplier == 1.8f);
    CHECK(bm.slider_tick_rate == 1.f);

    CHECK(bm.background == "Loundraw.full.214746799.jpg");
    CHECKED_IF(bm.breaks.size() == 1)
    {
        const auto brk = bm.breaks[0];
        CHECK(brk.first.count() == (75133ms).count());
        CHECK(brk.second.count() == (85060ms).count());
    }

    // Timing Points
    CHECKED_IF(bm.timingpoints.size() == 11)
    {
        {
            const auto tm = bm.timingpoints[0];
            CHECK(tm.time.count() == (267ms).count());
            CHECK(tm.beat_duration.count() == std::chrono::duration_cast<std::chrono::microseconds>(333.333333333333ms).count());
            CHECK(tm.meter == 4);
            CHECK(tm.sample_set == 2);
            CHECK(tm.sample_index == 1);
            CHECK(tm.sample_volume == 90);
            CHECK(tm.uninherited);
            CHECK(!tm.kiai);
        }
        {
            const auto tm = bm.timingpoints[1];
            CHECK(tm.time.count() == (21600ms).count());
            CHECK(tm.beat_duration.count() == std::chrono::duration_cast<std::chrono::microseconds>(277.777777777777ms).count());
            CHECK(tm.meter == 4);
            CHECK(tm.sample_set == 2);
            CHECK(tm.sample_index == 1);
            CHECK(tm.sample_volume == 90);
            CHECK(!tm.uninherited);
            CHECK(tm.kiai);
        }
        {
            const auto tm = bm.timingpoints[2];
            CHECK(tm.time.count() == (43266ms).count());
            CHECK(tm.beat_duration.count() == std::chrono::duration_cast<std::chrono::microseconds>(333.333333333333ms).count());
            CHECK(tm.meter == 4);
            CHECK(tm.sample_set == 2);
            CHECK(tm.sample_index == 1);
            CHECK(tm.sample_volume == 90);
            CHECK(!tm.uninherited);
            CHECK(!tm.kiai);
        }
    }

    // Hitobjects
    CHECK(bm.sliders.size() + bm.spinners.size() + bm.circles.size() == 403);
    CHECKED_IF(!bm.circles.empty())
    {
        const auto c = bm.circles.front();
        CHECK(c.pos.x == 104.f);
        CHECK(c.pos.y == 132.f);
        CHECK(c.time.count() == (267ms).count());
    }

    CHECKED_IF(!bm.sliders.empty())
    {
        //290,197,10933,6,0,P|242:197|170:205,1,90,0|0,0:0|0:0,0:0:0:0:
        const auto s = bm.sliders.front();
        CHECK(s.time.count() == (10933ms).count());
        CHECK(s.type == osu::Slider::Slider_type::perfect);
        CHECK(s.repeat == 1);
        CHECK(s.length == 90.f);
        CHECKED_IF(s.points.size() == 1)
        {
            CHECKED_IF(s.points[0].size() == 3)
            {
                CHECK(s.points[0][0].x == 290.f);
                CHECK(s.points[0][0].y == 197.f);
                CHECK(s.points[0][1].x == 242.f);
                CHECK(s.points[0][1].y == 197.f);
                CHECK(s.points[0][2].x == 170.f);
                CHECK(s.points[0][2].y == 205.f);
            }
        }
    }
}
