#include "sliderpath.h"
#include "bezier.h"
#include "catmull.h"
#include "perfect_circle.h"

// Heavily inspired by https://github.com/ppy/osu-framework/blob/99786238a02e0d6b69da86dd52e5506ee8ec0566/osu.Framework/Utils/PathApproximator.cs

static std::vector<osu::Vector2> approximate_linear(const std::vector<osu::Vector2>& control_points)
{
    return control_points;
}


std::vector<osu::Vector2> sliderpath(const osu::Slider& slider)
{
    std::vector<osu::Vector2> path;

    for(const auto& segment : slider.segments) {
        const auto points = [](const auto& segment) {
            switch(segment.type) {
                case osu::Slider::Slider_type::linear:
                    return approximate_linear(segment.points);
                case osu::Slider::Slider_type::perfect:
                    return approximate_perfect(segment.points);
                case osu::Slider::Slider_type::bezier:
                    return approximate_bezier(segment.points);
                case osu::Slider::Slider_type::catmull:
                    return approximate_catmull(segment.points);
            }
            // Maybe log in the future?
            return segment.points;
        }(segment);
        path.insert(path.end(), points.begin(), points.end());
    }

    return path;
}
