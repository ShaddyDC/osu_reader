#include "file_string.h"
#include <catch2/catch.hpp>
#include <osu_reader/beatmap_parser.h>
#include <osu_reader/hitobject_iterator.h>

template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

TEST_CASE("Beatmap Hitobject Iterator First")
{
    constexpr const auto filename =
            "res/Buta-Otome - Kakoi-naki Yo wa Ichigo no Tsukikage (BarkingMadDog) [this map is so bad cuz overmapping].osu";

    auto parser = osu::Beatmap_parser{};

    auto bm = parser.from_file(filename).value();

    auto it = osu::Hitobject_iterator(bm, overloaded{
                                                  [](const osu::Hitcircle& obj) { CHECK(obj.pos.x == 288.f); },
                                                  [](const auto&) { CHECK(false); }// Should be circle so we fail all else
                                          });

    it.next();
}

TEST_CASE("Beatmap Hitobject Iterator Count")
{
    constexpr const auto filename =
            "res/Buta-Otome - Kakoi-naki Yo wa Ichigo no Tsukikage (BarkingMadDog) [this map is so bad cuz overmapping].osu";

    auto parser = osu::Beatmap_parser{};

    auto bm = parser.from_file(filename).value();

    auto circles = 0;
    auto sliders = 0;
    auto spinners = 0;

    auto it = osu::Hitobject_iterator(bm, overloaded{
                                                  [&](const osu::Hitcircle&) { ++circles; },
                                                  [&](const osu::Slider&) { ++sliders; },
                                                  [&](const osu::Spinner&) { ++spinners; },
                                          });

    it.all();

    CHECK(circles == 801);
    CHECK(sliders == 110);
    CHECK(spinners == 1);

    CHECK(circles + sliders + spinners == 912);

    it.next();
}