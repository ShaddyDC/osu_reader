#include "parse_hitobject.h"
#include "parse_string.h"

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

    slider.control_points.push_back({parse_value<float>(tokens[x]), parse_value<float>(tokens[y])});

    parse_value(tokens[time], slider.time);
    parse_value(tokens[repeat], slider.repeat);
    parse_value(tokens[length], slider.length);

    // Parse slider type and points
    auto sub_tokens = split(tokens[slider_data], '|');
    if(sub_tokens.size() < 2) return std::nullopt;// Format: B|380:120|332:96|332:96|304:124
    std::transform(sub_tokens.begin(), sub_tokens.end(),
                   sub_tokens.begin(), ltrim_view);

    const auto valid_slider_type = [](const char slider_type) {
        const constexpr std::array<osu::Slider::Slider_type, 4> slider_types{
                osu::Slider::Slider_type::linear, osu::Slider::Slider_type::perfect,
                osu::Slider::Slider_type::bezier, osu::Slider::Slider_type::catmull};
        return std::any_of(slider_types.cbegin(), slider_types.cend(), [&](auto e) {
            return slider_type == static_cast<char>(e);
        });
    };
    if(sub_tokens[0].empty() || !valid_slider_type(sub_tokens[0][0])) return std::nullopt;
    slider.type = static_cast<osu::Slider::Slider_type>(sub_tokens[0][0]);

    std::transform(sub_tokens.cbegin() + 1, sub_tokens.cend(), std::back_inserter(slider.control_points), [](auto t) {
#if false
        Point point{};
        const auto pos = std::from_chars(t.data(), t.data() + t.length(), point.x).ptr;
        std::from_chars(pos + 1, t.data() + t.length(), point.y);
        return point;
#else// TODO: remove when from_chars is more widely supported
                osu::Point point{};
                std::size_t pos = 0;
                point.x = std::stof(&t.front(), &pos);
                point.y = std::stof(&t.front() + pos + 1, nullptr);
                return point;
#endif
    });

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
