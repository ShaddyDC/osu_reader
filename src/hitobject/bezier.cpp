#include "bezier.h"
#include "sliderpath.h"
#include <stack>

static bool bezier_is_flat_enough(const std::vector<osu::Vector2>& points);
static std::pair<std::vector<osu::Vector2>, std::vector<osu::Vector2>> bezier_subdivide(std::vector<osu::Vector2> points);
static void bezier_approximate(const std::vector<osu::Vector2>& points, std::back_insert_iterator<std::vector<osu::Vector2>> output_it);


// TODO: Consider reusing buffers
std::vector<osu::Vector2> approximate_bezier(const std::vector<osu::Vector2>& control_points)
{
    std::vector<osu::Vector2> points;

    if(control_points.empty()) return points;

    std::stack<std::vector<osu::Vector2>> to_flatten;

    to_flatten.push(control_points);

    while(!to_flatten.empty()) {
        const auto parent = to_flatten.top();
        to_flatten.pop();

        if(bezier_is_flat_enough(parent)) {
            bezier_approximate(parent, std::back_inserter(points));
            continue;
        }

        // Not const to allow moving
        auto [l, r] = bezier_subdivide(parent);

        to_flatten.push(l);
        to_flatten.push(r);
    }

    points.push_back(control_points.back());
    return points;
}

static bool bezier_is_flat_enough(const std::vector<osu::Vector2>& points)
{
    constexpr const float bezier_tolerance = 0.25f;
    for(auto i = 1; i < static_cast<int>(points.size()) - 1; ++i) {
        if(length_squared((points[i - 1] - 2 * points[i] + points[i + 1])) > bezier_tolerance)
            return false;
    }
    return true;
}

// Divide bezier curve into 2 separate curves of equal length
// If pieced together, they will result in the original curve
static std::pair<std::vector<osu::Vector2>, std::vector<osu::Vector2>> bezier_subdivide(std::vector<osu::Vector2> points)
{
    const auto count = static_cast<int>(points.size());
    std::vector<osu::Vector2> l(count), r(count);

    auto& midpoints = points;
    for(auto i = 0; i < count; ++i) {
        l[i] = midpoints[0];
        r[count - i - 1] = midpoints[count - i - 1];

        for(auto j = 0; j < count - i - 1; ++j) {
            midpoints[j] = midpoint(midpoints[j], midpoints[j + 1]);
        }
    }
    return {l, r};
}

static void bezier_approximate(const std::vector<osu::Vector2>& points, std::back_insert_iterator<std::vector<osu::Vector2>> output_it)
{
    const auto count = static_cast<int>(points.size());

    auto [l, r] = bezier_subdivide(points);
    l.insert(l.end(), r.begin() + 1, r.end());

    *output_it++ = points[0];

    for(auto i = 1; i < count - 1; ++i) {
        const auto index = 2 * i;
        *output_it = 0.25f * (l[index - 1] + 2 * l[index] + l[index + 1]);
    }
}