#include "osu_reader/beatmap_parser.h"
#include "string_stuff.h"
#include <charconv>
#include "util.h"
#include <array>
#include <variant>
#include "parse_string.h"

using Beatmap_types = std::variant<int osu::Beatmap::*, float osu::Beatmap::*, bool osu::Beatmap::*,
                                   std::string osu::Beatmap::*, std::chrono::milliseconds osu::Beatmap::*,
                                   std::filesystem::path osu::Beatmap::*, std::uint8_t osu::Beatmap::*,
                                   Gamemode osu::Beatmap::*,
                                   std::vector<std::chrono::milliseconds> osu::Beatmap::*,
                                   std::vector<std::string> osu::Beatmap::*>;

using Beatmap_match_pair = std::pair<const std::string_view, Beatmap_types>;

template<std::size_t N>
void if_found_parse(const std::array<Beatmap_match_pair, N>& map,
                    const std::string_view line_element_string, const std::string_view value_string,
                    osu::Beatmap& bm)
{
	if(const auto match = std::find_if(map.cbegin(), map.cend(),
	                                   [&line_element_string,
		                                   line_element = ltrim_view(line_element_string)](const auto& e)
	                                   {
		                                   return e.first == line_element_string;
	                                   });
		match != map.cend()){
		std::visit([&value_string, &bm](const auto& e)
		{
			parse_value(ltrim_view(value_string), bm.*e);
		}, match->second);
	}
}

void osu::Beatmap_parser::parse_general(const std::string_view line)
{
	static constexpr std::array<Beatmap_match_pair, 15> matcher{
		Beatmap_match_pair{ "AudioFilename", &Beatmap::audio_file },
		Beatmap_match_pair{ "AudioLeadIn", &Beatmap::audio_lead_in },
		Beatmap_match_pair{ "PreviewTime", &Beatmap::preview_time },
		Beatmap_match_pair{ "Countdown", &Beatmap::countdown },
		Beatmap_match_pair{ "SampleSet", &Beatmap::sample_set },
		Beatmap_match_pair{ "StackLeniency", &Beatmap::stack_leniency },
		Beatmap_match_pair{ "Mode", &Beatmap::mode },
		Beatmap_match_pair{ "LetterboxInBreaks", &Beatmap::letterbox_in_breaks },
		Beatmap_match_pair{ "StoryFireInFront", &Beatmap::story_fire_in_front },
		Beatmap_match_pair{ "SkinPreference", &Beatmap::skin_preference },
		Beatmap_match_pair{ "EpilepsyWarning", &Beatmap::epilepsy_warning },
		Beatmap_match_pair{ "CountdownOffset", &Beatmap::countdown_offset },
		Beatmap_match_pair{ "WidescreenStoryboard", &Beatmap::widescreen_storyboard },
		Beatmap_match_pair{ "SpecialStyle", &Beatmap::special_style },
		Beatmap_match_pair{ "UseSkinSprites", &Beatmap::use_skin_sprites },
	};

	const auto tokens = split(line, ':');
	if(tokens.size() != 2) return;

	if_found_parse(matcher, tokens[0], tokens[1], beatmap_);
}

void osu::Beatmap_parser::parse_editor(std::string_view line)
{
	static constexpr std::array<Beatmap_match_pair, 5> matcher{
		Beatmap_match_pair{ "Bookmarks", &Beatmap::bookmarks },
		Beatmap_match_pair{ "DistanceSpacing", &Beatmap::distance_spacing },
		Beatmap_match_pair{ "BeatDivisor", &Beatmap::beat_divisor },
		Beatmap_match_pair{ "GridSize", &Beatmap::grid_size },
		Beatmap_match_pair{ "TimelineZoom", &Beatmap::timeline_zoom },
	};

	const auto tokens = split(line, ':');
	if(tokens.size() != 2) return;

	if_found_parse(matcher, tokens[0], tokens[1], beatmap_);
}

void osu::Beatmap_parser::parse_metadata(std::string_view line)
{
	static constexpr std::array<Beatmap_match_pair, 10> matcher{
		Beatmap_match_pair{ "Title", &Beatmap::title },
		Beatmap_match_pair{ "TitleUnicode", &Beatmap::title_unicode },
		Beatmap_match_pair{ "Artist", &Beatmap::artist },
		Beatmap_match_pair{ "ArtistUnicode", &Beatmap::artist_unicode },
		Beatmap_match_pair{ "Creator", &Beatmap::creator },
		Beatmap_match_pair{ "Version", &Beatmap::difficulty_name },
		Beatmap_match_pair{ "Source", &Beatmap::source },
		Beatmap_match_pair{ "Tags", &Beatmap::tags },
		Beatmap_match_pair{ "BeatmapID", &Beatmap::beatmap_id },
		Beatmap_match_pair{ "BeatmapSetID", &Beatmap::beatmap_set_id },
	};

	const auto tokens = split(line, ':');
	if(tokens.size() != 2) return;

	if_found_parse(matcher, tokens[0], tokens[1], beatmap_);
}

void osu::Beatmap_parser::parse_difficulty(std::string_view line)
{
	static constexpr std::array<Beatmap_match_pair, 6> matcher{
		Beatmap_match_pair{ "HPDrainRate", &Beatmap::hp },
		Beatmap_match_pair{ "CircleSize", &Beatmap::cs },
		Beatmap_match_pair{ "OverallDifficulty", &Beatmap::od },
		Beatmap_match_pair{ "ApproachRate", &Beatmap::ar },
		Beatmap_match_pair{ "SliderMultiplier", &Beatmap::slider_multiplier },
		Beatmap_match_pair{ "SliderTickRate ", &Beatmap::slider_tick_rate },
	};

	const auto tokens = split(line, ':');
	if(tokens.size() != 2) return;

	if_found_parse(matcher, tokens[0], tokens[1], beatmap_);
}

void osu::Beatmap_parser::parse_events(std::string_view line)
{
	if(const std::string_view bg_prefix = "0,0,\""; // 0,0,"BG.png",0,0
		starts_with(line, bg_prefix)){
		const auto start = line.cbegin() + bg_prefix.length();
		const auto end   = std::find(start, line.cend(), '\"');
		if(end != line.cend())
			beatmap_.background = std::string{ start, end };
		return;
	}
	if(const std::string_view break_prefix = "2,"; // 2,start,end
		starts_with(line, break_prefix)){
		const std::string_view value_string = {
			line.data() + break_prefix.length(), line.length() - break_prefix.length()
		};
		auto tokens = split(value_string, ',');
		if(tokens.size() != 2) return;
		std::transform(tokens.begin(), tokens.end(), tokens.begin(), ltrim_view);
		beatmap_.breaks.emplace_back(parse_value<int>(tokens[0]), parse_value<int>(tokens[1]));
	}
}

void osu::Beatmap_parser::parse_timing_points(std::string_view line)
{
	enum Timingpoint_tokens{
		time,
		duration,
		meter,
		sample_set,
		sample_index,
		volume,
		inheritable,
		kiai
	};
	auto tokens = split(line, ',');
	if(tokens.size() < 8) return;
	std::transform(tokens.begin(), tokens.end(), tokens.begin(), ltrim_view);

	Beatmap::Timing_point point{};
	parse_value(tokens[time], point.time);

	if(const auto bpm_value = parse_value<float>(tokens[duration]);
		bpm_value < 0){
		point.inheritable           = false;
		const auto last_uninherited = std::find_if(beatmap_.timing_points.crbegin(),
		                                           beatmap_.timing_points.crend(),
		                                           [](auto p) { return p.inheritable; });
		if(last_uninherited != beatmap_.timing_points.crend()){
			point.beat_duration = std::chrono::duration_cast<std::chrono::microseconds>(
				0.01f * (-bpm_value) * last_uninherited->beat_duration);
		}
	} else{
		point.inheritable = true;
		using namespace std::chrono_literals;
		point.beat_duration = std::chrono::duration_cast<std::chrono::microseconds>(bpm_value * 1ms);
	}
	parse_value(tokens[meter], point.meter);
	parse_value(tokens[sample_set], point.sample_set);
	parse_value(tokens[sample_index], point.sample_index);
	parse_value(tokens[volume], point.sample_volume);
	parse_value(tokens[kiai], point.kiai);

	beatmap_.timing_points.push_back(point);
}

void osu::Beatmap_parser::parse_circle(const std::vector<std::string_view>& tokens)
{
	enum Circle_tokens{
		x,
		y,
		time,
		type,
		hitsound,
		extras
	};
	Hitcircle circle{};
	parse_value(tokens[x], circle.pos.x);
	parse_value(tokens[y], circle.pos.y);
	parse_value(tokens[time], circle.time);
	beatmap_.circles.push_back(circle);
}

void osu::Beatmap_parser::parse_slider(const std::vector<std::string_view>& tokens)
{
	enum Slider_tokens{
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

	if(tokens.size() < 8) return;
	Slider slider{};
	std::vector<Point> points;
	points.push_back({ parse_value<float>(tokens[x]), parse_value<float>(tokens[y]) });
	parse_value(tokens[time], slider.time);
	parse_value(tokens[repeat], slider.repeat);
	parse_value(tokens[length], slider.length);

	auto sub_tokens = split(tokens[slider_data], '|');
	if(sub_tokens.size() < 2) return; // Format: B|380:120|332:96|332:96|304:124
	std::transform(sub_tokens.begin(), sub_tokens.end(),
	               sub_tokens.begin(), ltrim_view);

	const auto valid_slider_type = [](const char slider_type)
	{
		const std::vector<Slider::Slider_type> slider_types{
			Slider::Slider_type::linear, Slider::Slider_type::perfect,
			Slider::Slider_type::bezier, Slider::Slider_type::catmull
		};
		return std::any_of(slider_types.cbegin(), slider_types.cend(), [&](auto e)
		{
			return slider_type == static_cast<char>(e);
		});
	};
	if(sub_tokens[0].empty() || !valid_slider_type(sub_tokens[0][0])) return;
	slider.type = static_cast<Slider::Slider_type>(sub_tokens[0][0]);

	std::transform(sub_tokens.cbegin() + 1, sub_tokens.cend(), std::back_inserter(points), [](auto t)
	{
		#if false
			Point point{};
			const auto pos = std::from_chars(t.data(), t.data() + t.length(), point.x).ptr;
			std::from_chars(pos + 1, t.data() + t.length(), point.y);
			return point;
		#else	// TODO: remove when from_chars is more widely supported
			Point point{};
			std::size_t pos = 0;
			point.x = std::stof(&t.front(), &pos);
			point.y = std::stof(&t.front() + pos + 1, nullptr);
			return point;
		#endif
	});

	if(!points.empty()){
		slider.points.emplace_back();
		slider.points.back().push_back(points.front());

		auto last_point = points.front();
		for(auto it = points.cbegin() + 1; it != points.cend(); ++it){
			if(it->x == last_point.x && it->y == last_point.y) slider.points.emplace_back();
			slider.points.back().push_back(*it);
		}
	}
	
	beatmap_.sliders.push_back(slider);
}

void osu::Beatmap_parser::parse_spinner(const std::vector<std::string_view>& tokens)
{
	enum Spinner_tokens{
		x,
		y,
		time,
		type,
		hitsound,
		end_time,
		extras
	};

	if(tokens.size() < 6) return;
	beatmap_.spinners.push_back({
		parse_value<std::chrono::milliseconds>(tokens[time]),
		parse_value<std::chrono::milliseconds>(tokens[end_time])
	});
}

void osu::Beatmap_parser::parse_hitobject(std::string_view line)
{
	constexpr auto type_token = 3;

	auto tokens = split(line, ',');
	if(tokens.size() < 4) return;
	std::transform(tokens.begin(), tokens.end(), tokens.begin(), ltrim_view);

	const auto type = parse_value<int>(tokens[type_token]);
	if(type & static_cast<int>(Hitobject_type::circle)){
		parse_circle(tokens);
	} else if(type & static_cast<int>(Hitobject_type::slider)){
		parse_slider(tokens);
	} else if(type & static_cast<int>(Hitobject_type::spinner)){
		parse_spinner(tokens);
	}
}

bool osu::Beatmap_parser::maybe_parse_utfheader(std::string_view line)
{
	using namespace util;

	const std::vector<std::vector<char>> potential_ge_utf16_headers{
		{ 0x00_ch, 0x00, 0xFE_ch, 0xFF_ch },    // utf32 BE
		{ 0xFF_ch, 0xFE_ch, 0x00_ch, 0x00_ch }, // utf32 LE
		{ 0xFE_ch, 0xFF_ch },                   // utf16 BE
		{ 0xFF_ch, 0xFE_ch },                   // utf16 LE
	};

	std::vector<char> header;
	std::copy_n(line.cbegin(),
	            std::min(4, static_cast<int>(line.size())),
	            std::back_inserter(header));

	const auto matches_header = [&header](const auto& e)
	{
		if(e.size() > header.size()) return false;
		return std::equal(e.cbegin(), e.cend(), header.cbegin());
	};

	if(const auto matching_header = std::find_if(potential_ge_utf16_headers.cbegin(), potential_ge_utf16_headers.cend(),
	                                             matches_header);
		matching_header != potential_ge_utf16_headers.cend()){
		return true;
	}
	
	return false;
}

void osu::Beatmap_parser::parse_line(const std::string_view line)
{
	if(line.empty()) return;

	if(starts_with(line, "[")) section_ = parse_section(line);
	else if(section_ == Section::general) parse_general(line);
	else if(section_ == Section::editor) parse_editor(line);
	else if(section_ == Section::metadata) parse_metadata(line);
	else if(section_ == Section::difficulty) parse_difficulty(line);
	else if(section_ == Section::events) parse_events(line);
	else if(section_ == Section::timing_points) parse_timing_points(line);
	else if(section_ == Section::hitobjects) parse_hitobject(line);
}

std::optional<osu::Beatmap> osu::Beatmap_parser::parse_impl(const std::function<std::optional<std::string>()>& line_provider)
{
	std::optional<std::string> line = line_provider();

	if(!line) return std::nullopt;

	using String_ref_fn = void(std::string&);
	const auto format_utf16 = maybe_parse_utfheader(*line)
		                          ? static_cast<String_ref_fn*>([](std::string& s)
		                          {
			                          s.erase(std::remove(s.begin(), s.end(), '\0'), s.end());
		                          })
		                          : static_cast<String_ref_fn*>([](std::string&) {});

	const auto seek_version_string = [&]
	{
		const std::string_view version_prefix = "osu file format v";
		do{
			format_utf16(*line);
			if(const auto pos = line->find(version_prefix);
				pos != std::string::npos){
				return pos + version_prefix.length();
			}
			line = line_provider();
		} while(line);
		return std::string::npos;
	};

	if(const auto prefix_pos = seek_version_string();
		prefix_pos == std::string::npos){
		return std::nullopt;
	} else{
		const std::string_view number_string = {
			line->data() + prefix_pos,
			line->length() - prefix_pos
		};

		if(const auto ec = std::from_chars(number_string.data(),
		                                   number_string.data() + number_string.length(), beatmap_.version).ec;
			ec == std::errc::invalid_argument || ec == std::errc::result_out_of_range){
			return std::nullopt;
		}
	}

	line = line_provider();
	while(line){
		format_utf16(*line);
		trim(*line);

		parse_line(*line);
		line = line_provider();
	}

	return beatmap_;
}

osu::Beatmap_parser::Section osu::Beatmap_parser::parse_section(std::string_view line)
{
	if(line == "[General]") return Section::general;
	if(line == "[Editor]") return Section::editor;
	if(line == "[Metadata]") return Section::metadata;
	if(line == "[Difficulty]") return Section::difficulty;
	if(line == "[Events]") return Section::events;
	if(line == "[TimingPoints]") return Section::timing_points;
	if(line == "[Colours]") return Section::colours;
	if(line == "[HitObjects]") return Section::hitobjects;
	return Section::none;
}
