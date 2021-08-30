#include "catmull.h"
#include "bezier.h"
#include "perfect_circle.h"
#include "sliderpath.h"
#include <cmath>
#include <stack>

static osu::Vector2 catmull_find_point(const osu::Vector2& v1, const osu::Vector2& v2, const osu::Vector2& v3, const osu::Vector2& v4, double t)
{
    const auto t2 = t * t;
    const auto t3 = t * t2;

    return {
            static_cast<float>(0.5 * (2 * v2.x + (-v1.x + v3.x) * t + (2 * v1.x - 5 * v2.x + 4 * v3.x - v4.x) * t2 + (-v1.x + 3 * v2.x - 3 * v3.x + v4.x) * t3)),
            static_cast<float>(0.5 * (2 * v2.y + (-v1.y + v3.y) * t + (2 * v1.y - 5 * v2.y + 4 * v3.y - v4.y) * t2 + (-v1.y + 3 * v2.y - 3 * v3.y + v4.y) * t3))};
}

std::vector<osu::Vector2> approximate_catmull(const std::vector<osu::Vector2>& control_points)
{
    const constexpr auto catmull_detail = 50;
    std::vector<osu::Vector2> points;
    points.reserve((control_points.size() - 1) * catmull_detail * 2);

    for(auto i = 0; i < static_cast<int>(control_points.size()) - 1; ++i) {
        const auto v1 = i > 0 ? control_points[i - 1] : control_points[i];
        const auto v2 = control_points[i];
        const auto v3 = i < static_cast<int>(control_points.size()) - 1 ? control_points[i + 1] : v2 + v2 - v1;
        const auto v4 = i < static_cast<int>(control_points.size()) - 2 ? control_points[i + 2] : v3 + v3 - v2;

        for(int c = 0; c < catmull_detail; ++c) {
            points.push_back(catmull_find_point(v1, v2, v3, v4, static_cast<float>(c) / catmull_detail));
            points.push_back(catmull_find_point(v1, v2, v3, v4, static_cast<float>(c + 1) / catmull_detail));
        }
    }
    return points;
}