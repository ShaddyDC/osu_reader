#include <catch2/catch.hpp>
#include <osu_reader/beatmap.h>
#include <timingpoints_helper.h>

TEST_CASE("Timingpoints find_next")
{
    using namespace std::chrono_literals;

    std::vector<osu::Beatmap::Timingpoint> timingpoints{
            {10000ms, std::chrono::microseconds{static_cast<int>(333.33f * 1000.f)}, 4, 0, 0, 100, true, true},
            {12000ms, std::chrono::microseconds{static_cast<int>(200.f * 1000.f)}, 4, 0, 0, 100, false, true},
            {15000ms, std::chrono::microseconds{static_cast<int>(300.f * 1000.f)}, 4, 0, 0, 100, false, true},
    };

    REQUIRE(osu::next_timingpoint(timingpoints.cbegin(), timingpoints.cend(), 9000ms)->time == 10000ms);
    REQUIRE(osu::next_timingpoint(timingpoints.cbegin(), timingpoints.cend(), 10000ms)->time == 10000ms);
    REQUIRE(osu::next_timingpoint(timingpoints.cbegin(), timingpoints.cend(), 11000ms)->time == 10000ms);
    REQUIRE(osu::next_timingpoint(timingpoints.cbegin(), timingpoints.cend(), 12000ms)->time == 12000ms);

    auto it = timingpoints.cbegin();
    REQUIRE(osu::next_timingpoint(it, timingpoints.cend(), 22000ms)->time == 15000ms);
    it = std::next(it);
    REQUIRE(osu::next_timingpoint(it, timingpoints.cend(), 22000ms)->time == 15000ms);
    it = std::next(it);
    REQUIRE(osu::next_timingpoint(it, timingpoints.cend(), 22000ms)->time == 15000ms);
    it = std::next(it);
    REQUIRE(it == timingpoints.cend());
}
