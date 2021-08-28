#include "parse_hitobject.h"
#include "parse_string.h"
#include <array>

std::optional<osu::Hitcircle> parse_circle(const std::vector<std::string_view>& tokens)
{
    enum Circle_tokens {
        x,
        y,
        time,
        type,
        hitsound,
        extras
    };
    osu::Hitcircle circle{};
    parse_value(tokens[x], circle.pos.x);
    parse_value(tokens[y], circle.pos.y);
    parse_value(tokens[time], circle.time);

    return circle;
}
std::optional<osu::Slider> parse_slider(const std::vector<std::string_view>& tokens)
{

    enum Slider_tokens {
        x,
        y,
        time,
        type,
        hitsound,
        slider_data,
        repeat,
        length,
        edge_hitsounds,
        edge_additions,
        extras
    };

    if(tokens.size() < 8) return std::nullopt;

    osu::Slider slider{};

    parse_value(tokens[time], slider.time);
    parse_value(tokens[repeat], slider.repeat);
    parse_value(tokens[length], slider.length);

    // Parse slider type and points
    auto sub_tokens = split(tokens[slider_data], '|');
    if(sub_tokens.size() < 2) return std::nullopt;// Format: B|380:120|332:96|332:96|304:124
    std::transform(sub_tokens.begin(), sub_tokens.end(),
                   sub_tokens.begin(), ltrim_view);

    const constexpr auto valid_slider_type = [](const char slider_type) {
        const static constexpr std::array<osu::Slider::Slider_type, 4> slider_types{
                osu::Slider::Slider_type::linear, osu::Slider::Slider_type::perfect,
                osu::Slider::Slider_type::bezier, osu::Slider::Slider_type::catmull};
        return std::any_of(slider_types.cbegin(), slider_types.cend(), [&](auto e) {
            return slider_type == static_cast<char>(e);
        });
    };
    if(sub_tokens[0].empty() || !valid_slider_type(sub_tokens[0][0])) return std::nullopt;

    // Transform points to curve segments
    slider.segments.emplace_back();
    slider.segments.back().type = slider.type = static_cast<osu::Slider::Slider_type>(sub_tokens[0].front());

    slider.segments.back().points.push_back({parse_value<float>(tokens[x]), parse_value<float>(tokens[y])});

    for(auto it = sub_tokens.cbegin() + 1; it != sub_tokens.cend(); ++it) {
        if(it->length() == 1) {// new slider type begin. Is this actually a thing though??
            slider.segments.emplace_back();
            slider.segments.back().type = static_cast<osu::Slider::Slider_type>(it->front());
            continue;
        }

        // TODO: Max coordinate values of 131072
#if false
        osu::Point point{};
        const auto pos = std::from_chars(it->data(), it->data() + it->length(), point.x).ptr;
        std::from_chars(pos + 1, it->data() + it->length(), point.y);
#else// TODO: remove when from_chars is more widely supported
        osu::Point point{};
        std::size_t pos = 0;
        point.x = std::stof(&it->front(), &pos);
        point.y = std::stof(&it->front() + pos + 1, nullptr);
#endif

        // Duplicate points means start of new segment
        if(!slider.segments.back().points.empty()) {
            if(const auto last_point = slider.segments.back().points.back();
               last_point.x == point.x && last_point.y == point.y) {

                // As in lazer, we only include the point in the new segment
                slider.segments.back().points.pop_back();
                const auto last_type = slider.segments.back().type;
                slider.segments.emplace_back();
                slider.segments.back().type = last_type;
                slider.segments.back().points.push_back(point);
                continue;
            }
        }

        slider.segments.back().points.push_back(point);
    }

    // Slider segment edge rules for perfect curves
    for(auto& segment : slider.segments) {
        if(segment.type != osu::Slider::Slider_type::perfect) continue;
        const auto& p = segment.points;
        if(p.size() != 3) segment.type = osu::Slider::Slider_type::bezier;
        else if(std::abs((p[1].y - p[0].y) * (p[2].x - p[0].x) - (p[1].x - p[0].x) * (p[2].y - p[0].y)) <= 1e-3f) {
            // Co-linear perfect curves to a linear path
            segment.type = osu::Slider::Slider_type::linear;
        }
    }

    return slider;
}
std::optional<osu::Spinner> parse_spinner(const std::vector<std::string_view>& tokens)
{
    enum Spinner_tokens {
        x,
        y,
        time,
        type,
        hitsound,
        end_time,
        extras
    };

    if(tokens.size() < 6) return std::nullopt;
    return osu::Spinner{parse_value<std::chrono::milliseconds>(tokens[time]),
                        parse_value<std::chrono::milliseconds>(tokens[end_time])};
}
