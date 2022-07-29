#include "osu_reader/sliderpath.h"
#include "bezier.h"
#include "catmull.h"
#include "perfect_circle.h"
#include <algorithm>

// Heavily inspired by https://github.com/ppy/osu-framework/blob/99786238a02e0d6b69da86dd52e5506ee8ec0566/osu.Framework/Utils/PathApproximator.cs

static std::vector<osu::Vector2> approximate_linear(const std::vector<osu::Vector2>& control_points)
{
    return control_points;
}


std::vector<osu::Vector2> osu::sliderpath(const osu::Slider& slider)
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

    // Remove duplicates
    path.erase(std::unique(path.begin(), path.end()), path.end());

    // Erase multiple points in same direction
    for(auto it = path.cbegin(); it != path.cend(); ++it) {
        if(normal(*(it - 2) - *(it - 1)) == normal(*(it - 1) - *it)) {
            // erase invalidates later iterators, so reassign
            it = path.erase(it - 1);
        }
    }

    return path;
}

std::vector<float> osu::pathlengths(const std::vector<osu::Vector2>& points)
{
    std::vector<float> distances{0};
    distances.reserve(points.size() + 1);

    auto distance = 0.f;
    for(auto i = 1; i < static_cast<signed>(points.size()); ++i) {
        distance += length((points[i] - points[i - 1]));
        distances.push_back(distance);
    }

    return distances;
}

void osu::fix_slider_length(osu::Slider& slider)
{
    if(slider.distances.empty()) return;
    if(slider.distances.size() != slider.points.size()) return;// TODO: Log or something?

    if(slider.distances.back() > slider.length) {
        while(slider.points.size() >= 2 && *(slider.distances.end() - 2) >= slider.length) {
            slider.distances.pop_back();
            slider.points.pop_back();
        }
        if(slider.points.size() >= 2 && slider.distances.back() > slider.length) {// Interpolate within the last segment
            const auto direction = normal(slider.points.back() - *(slider.points.end() - 2));
            slider.distances.pop_back();
            slider.points.pop_back();
            const auto length = slider.length - slider.distances.back();
            slider.distances.push_back(slider.length);
            slider.points.push_back(slider.points.back() + length * direction);
        }
    }
    if(slider.distances.back() < slider.length && slider.points.size() >= 2) {// Lengthen last segment
        const auto direction = normal(slider.points.back() - *(slider.points.end() - 2));
        const auto length = slider.length - slider.distances.back();
        slider.distances.push_back(slider.length);
        slider.points.push_back(slider.points.back() + length * direction);
    }
}
