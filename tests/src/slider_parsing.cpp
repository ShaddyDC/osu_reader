#include <catch2/catch.hpp>
#include <parse_hitobject.h>
#include <string_stuff.h>

using Segments = std::vector<osu::Slider::Segment>;

static bool operator==(const Segments& a, const Segments& b)
{
    if(a.size() != b.size()) return false;

    for(auto i = 0; i < a.size(); ++i) {
        if(a[i].type != b[i].type) return false;

        if(!std::equal(a[i].points.cbegin(), a[i].points.cend(), b[i].points.cbegin(), b[i].points.cend()))
            return false;
    }
    return true;
}

TEST_CASE("Linear Slider")
{
    const auto slider_string = "1,1,50000,2,0,L|5:1,7,4";
    const auto slider_tokens = split(slider_string, ',');
    const auto slider = parse_slider(slider_tokens);

    CHECKED_IF(slider)
    {
        CHECK(slider->type == osu::Slider::Slider_type::linear);
        CHECK(slider->length == 4);
        CHECK(slider->repeat == 7);
        CHECK(slider->time == std::chrono::milliseconds{50000});
        CHECK(slider->segments == Segments{{{{1, 1}, {5, 1}}, osu::Slider::Slider_type::linear}});
    }
}

TEST_CASE("Linear Slider Sanity Test")
{
    const auto slider_string = "1,1,50000,2,0,L|5:1,7,4";
    const auto slider_tokens = split(slider_string, ',');
    const auto slider = parse_slider(slider_tokens);

    CHECKED_IF(slider)
    {
        CHECK(slider->type == osu::Slider::Slider_type::linear);
        CHECK(slider->length == 4);
        CHECK(slider->repeat == 7);
        CHECK(slider->time == std::chrono::milliseconds{50000});
        CHECK_FALSE(slider->segments == Segments{{{{1, 1}, {5, 1}, {3, 3}}, osu::Slider::Slider_type::linear}});
    }
}

TEST_CASE("Linear Slider & Linear Slider")
{
    const auto slider_string = "1,1,50000,2,0,L|5:1|L|3:1|6:1,7,9";
    const auto slider_tokens = split(slider_string, ',');
    const auto slider = parse_slider(slider_tokens);

    CHECKED_IF(slider)
    {
        CHECK(slider->type == osu::Slider::Slider_type::linear);
        CHECK(slider->length == 9);
        CHECK(slider->repeat == 7);
        CHECK(slider->time == std::chrono::milliseconds{50000});
        CHECK(slider->segments == Segments{{{{1, 1}, {5, 1}}, osu::Slider::Slider_type::linear},
                                           {{{3, 1}, {6, 1}}, osu::Slider::Slider_type::linear}});
    }
}

TEST_CASE("Linear Slider Double Point")
{
    const auto slider_string = "1,1,50000,2,0,L|5:1|3:1|3:1|6:1,7,9";
    const auto slider_tokens = split(slider_string, ',');
    const auto slider = parse_slider(slider_tokens);

    CHECKED_IF(slider)
    {
        CHECK(slider->type == osu::Slider::Slider_type::linear);
        CHECK(slider->length == 9);
        CHECK(slider->repeat == 7);
        CHECK(slider->time == std::chrono::milliseconds{50000});
        CHECK(slider->segments == Segments{{{{1, 1}, {5, 1}, {3, 1}}, osu::Slider::Slider_type::linear},
                                           {{{3, 1}, {6, 1}}, osu::Slider::Slider_type::linear}});
    }
}

TEST_CASE("Bezier Slider")
{
    const auto slider_string = "1,1,50000,2,0,B|5:1,7,4";
    const auto slider_tokens = split(slider_string, ',');
    const auto slider = parse_slider(slider_tokens);

    CHECKED_IF(slider)
    {
        CHECK(slider->type == osu::Slider::Slider_type::bezier);
        CHECK(slider->length == 4);
        CHECK(slider->repeat == 7);
        CHECK(slider->time == std::chrono::milliseconds{50000});
        CHECK(slider->segments == Segments{{{{1, 1}, {5, 1}}, osu::Slider::Slider_type::bezier}});
    }
}

TEST_CASE("Catmull Slider")
{
    const auto slider_string = "1,1,50000,2,0,C|8:8|5:1,7,4";
    const auto slider_tokens = split(slider_string, ',');
    const auto slider = parse_slider(slider_tokens);

    CHECKED_IF(slider)
    {
        CHECK(slider->type == osu::Slider::Slider_type::catmull);
        CHECK(slider->length == 4);
        CHECK(slider->repeat == 7);
        CHECK(slider->time == std::chrono::milliseconds{50000});
        CHECK(slider->segments == Segments{{{{1, 1}, {8, 8}, {5, 1}}, osu::Slider::Slider_type::catmull}});
    }
}

TEST_CASE("Perfect Curve Slider")
{
    const auto slider_string = "1,1,50000,2,0,P|8:8|5:1,7,4";
    const auto slider_tokens = split(slider_string, ',');
    const auto slider = parse_slider(slider_tokens);

    CHECKED_IF(slider)
    {
        CHECK(slider->type == osu::Slider::Slider_type::perfect);
        CHECK(slider->length == 4);
        CHECK(slider->repeat == 7);
        CHECK(slider->time == std::chrono::milliseconds{50000});
        CHECK(slider->segments == Segments{{{{1, 1}, {8, 8}, {5, 1}}, osu::Slider::Slider_type::perfect}});
    }
}

TEST_CASE("Perfect Curve Slider with 2 points (bezier)")
{
    const auto slider_string = "1,1,50000,2,0,P|5:1,7,4";
    const auto slider_tokens = split(slider_string, ',');
    const auto slider = parse_slider(slider_tokens);

    CHECKED_IF(slider)
    {
        CHECK(slider->type == osu::Slider::Slider_type::perfect);
        CHECK(slider->length == 4);
        CHECK(slider->repeat == 7);
        CHECK(slider->time == std::chrono::milliseconds{50000});
        CHECK(slider->segments == Segments{{{{1, 1}, {5, 1}}, osu::Slider::Slider_type::bezier}});
    }
}

TEST_CASE("Perfect Curve Slider with 4 points (bezier)")
{
    const auto slider_string = "1,1,50000,2,0,P|8:8|9:9|5:1,7,4";
    const auto slider_tokens = split(slider_string, ',');
    const auto slider = parse_slider(slider_tokens);

    CHECKED_IF(slider)
    {
        CHECK(slider->type == osu::Slider::Slider_type::perfect);
        CHECK(slider->length == 4);
        CHECK(slider->repeat == 7);
        CHECK(slider->time == std::chrono::milliseconds{50000});
        CHECK(slider->segments == Segments{{{{1, 1}, {8, 8}, {9, 9}, {5, 1}}, osu::Slider::Slider_type::bezier}});
    }
}

TEST_CASE("Perfect Curve Slider co-linear to linear")
{
    const auto slider_string = "1,1,50000,2,0,P|3:1|5:1,7,4";
    const auto slider_tokens = split(slider_string, ',');
    const auto slider = parse_slider(slider_tokens);

    CHECKED_IF(slider)
    {
        CHECK(slider->type == osu::Slider::Slider_type::perfect);
        CHECK(slider->length == 4);
        CHECK(slider->repeat == 7);
        CHECK(slider->time == std::chrono::milliseconds{50000});
        CHECK(slider->segments == Segments{{{{1, 1}, {3, 1}, {5, 1}}, osu::Slider::Slider_type::linear}});
    }
}
