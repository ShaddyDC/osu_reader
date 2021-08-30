#include "sliderpath.h"
#include <cmath>
#include <stack>

// Heavily inspired by https://github.com/ppy/osu-framework/blob/99786238a02e0d6b69da86dd52e5506ee8ec0566/osu.Framework/Utils/PathApproximator.cs

static std::vector<osu::Point> approximate_linear(const std::vector<osu::Point>& control_points)
{
    return control_points;
}

static void bezier_approximate(const std::vector<osu::Point>& points, std::back_insert_iterator<std::vector<osu::Point>> output_it);
static bool bezier_is_flat_enough(const std::vector<osu::Point>& points);
static std::pair<std::vector<osu::Point>, std::vector<osu::Point>> bezier_subdivide(std::vector<osu::Point> points);

static std::vector<osu::Point> approximate_bezier(const std::vector<osu::Point>& control_points)
{
    std::vector<osu::Point> points;

    if(control_points.empty()) return points;

    std::stack<std::vector<osu::Point>> to_flatten;

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

static bool bezier_is_flat_enough(const std::vector<osu::Point>& points)
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
static std::pair<std::vector<osu::Point>, std::vector<osu::Point>> bezier_subdivide(std::vector<osu::Point> points)
{
    const auto count = static_cast<int>(points.size());
    std::vector<osu::Point> l(count), r(count);

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

static void bezier_approximate(const std::vector<osu::Point>& points, std::back_insert_iterator<std::vector<osu::Point>> output_it)
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

struct Circular_properties {
    bool is_valid = false;
    double theta_start = 0;
    double theta_range = 0;
    double direction = 0;
    float radius = 0;
    osu::Point centre = {0, 0};
};

Circular_properties circular_properties(const std::vector<osu::Point>& control_points)
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

    const auto centre = osu::Point{
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

static std::vector<osu::Point> approximate_perfect(const std::vector<osu::Point>& control_points)
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

    std::vector<osu::Point> points;
    for(auto i = 0; i < point_count; ++i) {
        const auto fract = static_cast<double>(i) / (point_count - 1);
        const auto theta = properties.theta_start + properties.direction * fract * properties.theta_range;
        points.push_back(properties.centre + properties.radius * osu::Point{static_cast<float>(std::cos(theta)), static_cast<float>(std::sin(theta))});
    }

    // Failure for some reason. Maybe worth logging to investigate? Same behaviour as lazer
    if(points.empty()) return approximate_bezier(control_points);

    return points;
}

static osu::Point catmull_find_point(const osu::Point& v1, const osu::Point& v2, const osu::Point& v3, const osu::Point& v4, double t)
{
    const auto t2 = t * t;
    const auto t3 = t * t2;

    return {
            static_cast<float>(0.5 * (2 * v2.x + (-v1.x + v3.x) * t + (2 * v1.x - 5 * v2.x + 4 * v3.x - v4.x) * t2 + (-v1.x + 3 * v2.x - 3 * v3.x + v4.x) * t3)),
            static_cast<float>(0.5 * (2 * v2.y + (-v1.y + v3.y) * t + (2 * v1.y - 5 * v2.y + 4 * v3.y - v4.y) * t2 + (-v1.y + 3 * v2.y - 3 * v3.y + v4.y) * t3))};
}

static std::vector<osu::Point> approximate_catmull(const std::vector<osu::Point>& control_points)
{
    const constexpr auto catmull_detail = 50;
    std::vector<osu::Point> points;
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

std::vector<osu::Point> sliderpath(const osu::Slider& slider)
{
    std::vector<osu::Point> path;

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
