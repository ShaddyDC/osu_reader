
#include "perfect_circle.h"
#include "bezier.h"
#include <cmath>
#include <stack>
struct Circular_properties {
    bool is_valid = false;
    double theta_start = 0;
    double theta_range = 0;
    double direction = 0;
    float radius = 0;
    osu::Vector2 centre = {0, 0};
};
Circular_properties circular_properties(const std::vector<osu::Vector2>& control_points)
{
    if(control_points.size() != 3) return {};

    const auto a = control_points[0];
    const auto b = control_points[1];
    const auto c = control_points[2];

    // Degenerate triangle with side-length almost zero - fall back to numerically stable method
    if(std::abs((b.y - a.y) * (c.x - a.x) - (b.x - a.x) * (c.y - a.y)) <= 1e-3f)
        return {};

    const auto d = 2.f * (a.x * (b - c).y + b.x * (c - a).y + c.x * (a - b).y);
    const auto a_sq = length_squared(a);
    const auto b_sq = length_squared(b);
    const auto c_sq = length_squared(c);

    const auto centre = osu::Vector2{
            (a_sq * (b - c).y + b_sq * (c - a).y + c_sq * (a - b).y) / d,
            (a_sq * (c - b).x + b_sq * (a - c).x + c_sq * (b - a).x) / d};

    const auto d_a = a - centre;
    const auto d_c = c - centre;

    const auto r = length(d_a);

    const auto theta_start = std::atan2(d_a.y, d_a.x);
    auto theta_end = std::atan2(d_c.y, d_c.x);
    while(theta_end < theta_start) theta_end += 2 * M_PI;

    auto dir = 1.;
    auto theta_range = theta_end - theta_start;

    // Decide circle direction, depending on which side of AC B lies
    auto ortho_a_to_c = c - a;
    ortho_a_to_c = {ortho_a_to_c.y, -ortho_a_to_c.x};

    if(dot(ortho_a_to_c, b - a) < 0) {
        dir = -dir;
        theta_range = static_cast<float>(2 * M_PI) - theta_range;
    }
    return Circular_properties{true, theta_start, theta_range, dir, r, centre};
}

std::vector<osu::Vector2> approximate_perfect(const std::vector<osu::Vector2>& control_points)
{
    constexpr const auto circular_arc_tolerance = 0.1;

    const auto properties = circular_properties(control_points);
    if(!properties.is_valid) return approximate_bezier(control_points);

    // To quote peppy:
    // We select the amount of points for the approximation by requiring the discrete curvature
    // to be smaller than the provided tolerance. The exact angle required to meet the tolerance
    // is: 2 * Math.Acos(1 - TOLERANCE / r)
    // The special case is required for extremely short sliders where the radius is smaller than
    // the tolerance. This is a pathological rather than a realistic case.
    const auto point_count = 2 * properties.radius <= circular_arc_tolerance
                                     ? 2
                                     : std::max(2., std::ceil(properties.theta_range / (2 * std::acos(1 - circular_arc_tolerance / properties.radius))));

    std::vector<osu::Vector2> points;
    for(auto i = 0; i < point_count; ++i) {
        const auto fract = static_cast<double>(i) / (point_count - 1);
        const auto theta = properties.theta_start + properties.direction * fract * properties.theta_range;
        points.push_back(properties.centre + properties.radius * osu::Vector2{static_cast<float>(std::cos(theta)), static_cast<float>(std::sin(theta))});
    }

    // Failure for some reason. Maybe worth logging to investigate? Same behaviour as lazer
    if(points.empty()) return approximate_bezier(control_points);

    return points;
}
