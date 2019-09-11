#include "osu_reader/beatmap.h"
#include "string_stuff.h"
#include <charconv>
#include "util.h"
#include <map>
#include <variant>
#include "parse_string.h"

using Beatmap_types = std::variant<int*, float*, bool*, std::string*,
                                   std::chrono::milliseconds*, std::filesystem::path*, std::uint8_t*, Gamemode*,
                                   std::vector<std::chrono::milliseconds>*, std::vector<std::string>*>;

std::optional<osu::Beatmap_file> osu::Beatmap_parser::parse(const std::filesystem::path& file)
{
	return Beatmap_parser{ file }.parse_impl();
}

osu::Beatmap_parser::Beatmap_parser(const std::filesystem::path& file)
	: file_{ file } {}

void osu::Beatmap_parser::parse_general(const std::string_view line)
{
	const std::map<std::string_view, Beatmap_types> matcher{
		{ "AudioFilename", &beatmap_.audio_file },
		{ "AudioLeadIn", &beatmap_.audio_lead_in },
		{ "PreviewTime", &beatmap_.preview_time },
		{ "Countdown", &beatmap_.countdown },
		{ "SampleSet", &beatmap_.sample_set },
		{ "StackLeniency", &beatmap_.stack_leniency },
		{ "Mode", &beatmap_.mode },
		{ "LetterboxInBreaks", &beatmap_.letterbox_in_breaks },
		{ "StoryFireInFront", &beatmap_.story_fire_in_front },
		{ "SkinPreference", &beatmap_.skin_preference },
		{ "EpilepsyWarning", &beatmap_.epilepsy_warning },
		{ "CountdownOffset", &beatmap_.countdown_offset },
		{ "WidescreenStoryboard", &beatmap_.widescreen_storyboard },
		{ "SpecialStyle", &beatmap_.special_style },
		{ "UseSkinSprites", &beatmap_.use_skin_sprites },
	};

	const auto tokens = split(line, ':');
	if(tokens.size() != 2) return;

	if(const auto match = matcher.find(rtrim_view(tokens[0]));
		match != matcher.end()){
		std::visit([&tokens](const auto& e)
		{
			parse_value(ltrim_view(tokens[1]), *e);
		}, match->second);
	}
}

void osu::Beatmap_parser::parse_editor(std::string_view line)
{
	const std::map<std::string_view, Beatmap_types> matcher{
		{ "Bookmarks", &beatmap_.bookmarks },
		{ "DistanceSpacing", &beatmap_.distance_spacing },
		{ "BeatDivisor", &beatmap_.beat_divisor },
		{ "GridSize", &beatmap_.grid_size },
		{ "TimelineZoom", &beatmap_.timeline_zoom },
	};

	const auto tokens = split(line, ':');
	if(tokens.size() != 2) return;

	if(const auto match = matcher.find(rtrim_view(tokens[0]));
		match != matcher.end()){
		std::visit([&tokens](const auto& e)
		{
			parse_value(ltrim_view(tokens[1]), *e);
		}, match->second);
	}
}

void osu::Beatmap_parser::parse_metadata(std::string_view line)
{
	const std::map<std::string_view, Beatmap_types> matcher{
		{ "Title", &beatmap_.title },
		{ "TitleUnicode", &beatmap_.title_unicode },
		{ "Artist", &beatmap_.artist },
		{ "ArtistUnicode", &beatmap_.artist_unicode },
		{ "Creator", &beatmap_.creator },
		{ "Version", &beatmap_.difficulty_name },
		{ "Source", &beatmap_.source },
		{ "Tags", &beatmap_.tags },
		{ "BeatmapID", &beatmap_.beatmap_id },
		{ "BeatmapSetID", &beatmap_.beatmap_set_id },
	};

	const auto tokens = split(line, ':');
	if(tokens.size() != 2) return;

	if(const auto match = matcher.find(rtrim_view(tokens[0]));
		match != matcher.end()){
		std::visit([&tokens](const auto& e)
		{
			parse_value(ltrim_view(tokens[1]), *e);
		}, match->second);
	}
}

void osu::Beatmap_parser::parse_difficulty(std::string_view line)
{
	const std::map<std::string_view, Beatmap_types> matcher{
		{ "HPDrainRate", &beatmap_.hp },
		{ "CircleSize", &beatmap_.cs },
		{ "OverallDifficulty", &beatmap_.od },
		{ "ApproachRate", &beatmap_.ar },
		{ "SliderMultiplier", &beatmap_.slider_multiplier },
		{ "SliderTickRate ", &beatmap_.slider_tick_rate },
	};

	const auto tokens = split(line, ':');
	if(tokens.size() != 2) return;

	if(const auto match = matcher.find(rtrim_view(tokens[0]));
		match != matcher.end()){
		std::visit([&tokens](const auto& e)
		{
			parse_value(ltrim_view(tokens[1]), *e);
		}, match->second);
	}
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
		std::for_each(tokens.begin(), tokens.end(), ltrim_view);
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
	const auto tokens = split(line, ',');
	if(tokens.size() < 8) return;
	std::for_each(tokens.begin(), tokens.end(), ltrim_view);

	Beatmap_file::Timing_point point{};
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
	slider.points.push_back({ parse_value<float>(tokens[x]), parse_value<float>(tokens[y]) });
	parse_value(tokens[time], slider.time);
	parse_value(tokens[repeat], slider.repeat);
	parse_value(tokens[length], slider.length);

	const auto sub_tokens = split(tokens[slider_data], '|');
	if(sub_tokens.size() < 2) return; // Format: B|380:120|332:96|332:96|304:124
	std::for_each(sub_tokens.begin(), sub_tokens.end(), ltrim_view);

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

	std::transform(sub_tokens.cbegin() + 1, sub_tokens.cend(), std::back_inserter(slider.points), [](auto t)
	{
		Point point{};
		const auto pos = std::from_chars(t.data(), t.data() + t.length(), point.x).ptr;
		std::from_chars(pos + 1, t.data() + t.length(), point.y);
		return point;
	});
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

	const auto tokens = split(line, ',');
	if(tokens.size() < 4) return;
	std::for_each(tokens.begin(), tokens.end(), ltrim_view);

	const auto type = parse_value<int>(tokens[type_token]);
	if(type & static_cast<int>(Hitobject_type::circle)){
		parse_circle(tokens);
	} else if(type & static_cast<int>(Hitobject_type::slider)){
		parse_slider(tokens);
	} else if(type & static_cast<int>(Hitobject_type::spinner)){
		parse_spinner(tokens);
	}
}

bool osu::Beatmap_parser::maybe_parse_utfheader()
{
	std::vector<char> header;
	std::generate_n(std::back_inserter(header), 4, [this] { return static_cast<char>(file_.get()); });

	using namespace util;

	const std::vector<std::vector<char>> potential_ge_utf16_headers{
		{ 0x00_ch, 0x00, 0xFE_ch, 0xFF_ch },    // utf32 BE
		{ 0xFF_ch, 0xFE_ch, 0x00_ch, 0x00_ch }, // utf32 LE
		{ 0xFE_ch, 0xFF_ch },                   // utf16 BE
		{ 0xFF_ch, 0xFE_ch },                   // utf16 LE
	};

	const auto matches_header = [&header](const auto& e)
	{
		return std::equal(e.cbegin(), e.cend(), header.cbegin());
	};

	if(const auto matching_header = std::find_if(potential_ge_utf16_headers.cbegin(), potential_ge_utf16_headers.cend(),
	                                             matches_header);
		matching_header != potential_ge_utf16_headers.cend()){
		if(matching_header->size() < header.size()) file_.seekg(matching_header->size());
		return true;
	}

	const std::vector<char> utf8_header{ 0xFF_ch, 0xBB_ch, 0xBF_ch };
	if(const auto is_utf8 = std::equal(utf8_header.cbegin(), utf8_header.cend(), header.cbegin());
		is_utf8){
		file_.seekg(utf8_header.size());
		return false;
	}
	file_.seekg(0);
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

std::optional<osu::Beatmap_file> osu::Beatmap_parser::parse_impl()
{
	if(!file_.is_open()) return std::nullopt;

	using String_ref_fn = void(std::string&);
	const auto format_utf16 = maybe_parse_utfheader()
		                          ? static_cast<String_ref_fn*>([](std::string& s)
		                          {
			                          s.erase(std::remove(s.begin(), s.end(), '\0'), s.end());
		                          })
		                          : static_cast<String_ref_fn*>([](std::string&) {});

	std::string line;
	const std::string_view version_prefix = "osu file format v";

	const auto seek_version_string = [&]
	{
		while(std::getline(file_, line)){
			format_utf16(line);
			if(const auto pos = line.find(version_prefix);
				pos != std::string::npos){
				return pos;
			}
		}
		return std::string::npos;
	};

	if(const auto prefix_pos = seek_version_string();
		prefix_pos == std::string::npos){
		return std::nullopt;
	} else{
		const std::string_view number_string = {
			line.data() + prefix_pos + version_prefix.length(),
			line.length() - version_prefix.length() - prefix_pos
		};

		if(const auto ec = std::from_chars(number_string.data(),
		                                   number_string.data() + number_string.length(), beatmap_.version).ec;
			ec == std::errc::invalid_argument || ec == std::errc::result_out_of_range){
			return std::nullopt;
		}
	}

	while(std::getline(file_, line)){
		format_utf16(line);
		trim(line);

		parse_line(line);
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
