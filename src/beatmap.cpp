#include "osu_reader/beatmap.h"
#include "string_stuff.h"
#include <charconv>
#include "util.h"

template<typename Type>
bool maybe_parse(std::string_view line, std::string_view prefix, Type& value)
{
	if(starts_with(line, prefix)){
		const auto value_string = ltrim_view({ line.data() + prefix.length(), line.length() - prefix.length() });
		std::from_chars(value_string.data(), value_string.data() + value_string.length(), value);
		return true;
	}
	return false;
}

template<>
bool maybe_parse<>(std::string_view line, std::string_view prefix, bool& value)
{
	if(starts_with(line, prefix)){
		const auto value_string = ltrim_view({ line.data() + prefix.length(), line.length() - prefix.length() });
		if(!value_string.empty()){
			if(value_string[0] == '1') value = true;
			else if(value_string[0] == '0') value = false;
		}
		return true;
	}
	return false;
}

template<>
bool maybe_parse<>(std::string_view line, std::string_view prefix, std::chrono::milliseconds& value)
{
	auto v = 0;
	if(maybe_parse(line, prefix, v)){
		value = std::chrono::milliseconds{ v };
		return true;
	}
	return false;
}

template<>
bool maybe_parse<>(std::string_view line, std::string_view prefix, Gamemode& value)
{
	auto v = 0;
	if(maybe_parse(line, prefix, v)){
		value = static_cast<Gamemode>(value);
		return true;
	}
	return false;
}

template<>
bool maybe_parse<>(std::string_view line, std::string_view prefix, std::string& value)
{
	if(starts_with(line, prefix)){
		value = ltrim_view({ line.data() + prefix.length(), line.length() - prefix.length() });
		return true;
	}
	return false;
}

template<>
bool maybe_parse<>(std::string_view line, std::string_view prefix, std::filesystem::path& value)
{
	std::string s;
	if(maybe_parse(line, prefix, s)){
		value = s;
		return true;
	}
	return false;
}

std::optional<osu::Beatmap_file> osu::Beatmap_parser::parse(const std::filesystem::path& file)
{
	return Beatmap_parser{ file }.parse_impl();
}

osu::Beatmap_parser::Beatmap_parser(const std::filesystem::path& file)
	: file_{ file } {}

bool osu::Beatmap_parser::parse_general(const std::string_view line)
{
	// Try until first match is parsed
	return !maybe_parse(line, "AudioFilename:", beatmap_.audio_file)
			&& !maybe_parse(line, "AudioLeadIn:", beatmap_.audio_lead_in)
			&& !maybe_parse(line, "PreviewTime:", beatmap_.preview_time)
			&& !maybe_parse(line, "Countdown:", beatmap_.countdown)
			&& !maybe_parse(line, "SampleSet:", beatmap_.sample_set)
			&& !maybe_parse(line, "StackLeniency:", beatmap_.stack_leniency)
			&& !maybe_parse(line, "Mode:", beatmap_.mode)
			&& !maybe_parse(line, "LetterboxInBreaks:", beatmap_.letterbox_in_breaks)
			&& !maybe_parse(line, "StoryFireInFront:", beatmap_.story_fire_in_front)
			&& !maybe_parse(line, "SkinPreference:", beatmap_.skin_preference)
			&& !maybe_parse(line, "EpilepsyWarning:", beatmap_.epilepsy_warning)
			&& !maybe_parse(line, "CountdownOffset:", beatmap_.countdown_offset)
			&& !maybe_parse(line, "WidescreenStoryboard:", beatmap_.widescreen_storyboard)
			&& !maybe_parse(line, "SpecialStyle:", beatmap_.special_style)
			&& !maybe_parse(line, "UseSkinSprites:", beatmap_.use_skin_sprites);
}

bool osu::Beatmap_parser::maybe_parse_bookmarks(std::string_view line)
{
	const std::string_view prefix = "Bookmarks:";
	if(!starts_with(line, prefix)){
		return false;
	}
	const auto value = ltrim_view({ line.data() + prefix.length(), line.length() - prefix.length() });
	for(const auto token : split(value, ' ')){
		const auto t = ltrim_view(token);
		int v        = 0;
		if(const auto ec = std::from_chars(t.data(), t.data() + t.length(), v);
			ec.ec != std::errc::invalid_argument && ec.ec != std::errc::result_out_of_range){
			beatmap_.bookmarks.emplace_back(v);
		}
	}

	return true;
}

bool osu::Beatmap_parser::parse_editor(std::string_view line)
{
	return !maybe_parse_bookmarks(line)
			&& !maybe_parse(line, "DistanceSpacing:", beatmap_.distance_spacing)
			&& !maybe_parse(line, "BeatDivisor:", beatmap_.beat_divisor)
			&& !maybe_parse(line, "GridSize:", beatmap_.grid_size)
			&& !maybe_parse(line, "TimelineZoom:", beatmap_.timeline_zoom);
}

bool osu::Beatmap_parser::parse_metadata(std::string_view line)
{
	return !maybe_parse(line, "Title:", beatmap_.title)
			&& !maybe_parse(line, "TitleUnicode:", beatmap_.title_unicode)
			&& !maybe_parse(line, "Artist:", beatmap_.artist)
			&& !maybe_parse(line, "ArtistUnicode:", beatmap_.artist_unicode)
			&& !maybe_parse(line, "Creator:", beatmap_.creator)
			&& !maybe_parse(line, "Version:", beatmap_.difficulty_name)
			&& !maybe_parse(line, "Source:", beatmap_.source)
			//&& !maybe_parse(line, "Tags:", beatmap_.tags)
			&& !maybe_parse(line, "BeatmapID:", beatmap_.beatmap_id)
			&& !maybe_parse(line, "BeatmapSetID:", beatmap_.beatmap_set_id);
}

bool osu::Beatmap_parser::parse_difficulty(std::string_view line)
{
	return !maybe_parse(line, "HPDrainRate:", beatmap_.hp)
			&& !maybe_parse(line, "CircleSize:", beatmap_.cs)
			&& !maybe_parse(line, "OverallDifficulty:", beatmap_.od)
			&& !maybe_parse(line, "ApproachRate:", beatmap_.ar)
			&& !maybe_parse(line, "SliderMultiplier:", beatmap_.slider_multiplier)
			&& !maybe_parse(line, "SliderTickRate :", beatmap_.slider_tick_rate);
}

bool osu::Beatmap_parser::parse_events(std::string_view line)
{
	if(const std::string_view bg_prefix = "0,0,\"", // 0,0,"BG.png",0,0
	                          bg_suffix = "\",0,0";
		starts_with(line, bg_prefix)){
		beatmap_.background = std::string{ line.cbegin() + bg_prefix.length(), line.cend() - bg_suffix.length() };
		return true;
	}
	if(const std::string_view break_prefix = "2,"; // 2,start,end
		starts_with(line, break_prefix)){
		const std::string_view value_string = {
			line.data() + break_prefix.length(), line.length() - break_prefix.length()
		};
		auto tokens = split(value_string, ',');
		if(tokens.size() != 2) return true;
		std::for_each(tokens.begin(), tokens.end(), ltrim_view);
		int a, b;
		std::from_chars(tokens[0].data(), tokens[0].data() + tokens[0].length(), a);
		std::from_chars(tokens[1].data(), tokens[1].data() + tokens[1].length(), b);
		beatmap_.breaks.emplace_back(a, b);
		return true;
	}
	return false;
}


bool osu::Beatmap_parser::parse_timing_points(std::string_view line)
{
	const auto tokens = split(line, ',');
	if(tokens.size() < 8) return true;
	std::for_each(tokens.begin(), tokens.end(), ltrim_view);

	const auto parse_i = [&](auto i, auto& v)
	{
		std::from_chars(tokens[i].data(), tokens[i].data() + tokens[i].length(), v);
	};

	Beatmap_file::Timing_point point{};
	int v0;
	parse_i(0, v0);
	point.time = std::chrono::milliseconds{ v0 };
	float v1;
	parse_i(1, v1);
	if(v1 < 0){
		point.inheritable           = false;
		const auto last_uninherited = std::find_if(beatmap_.timing_points.crbegin(), beatmap_.timing_points.crend(),
		                                           [](auto p) { return p.inheritable; });
		if(last_uninherited != beatmap_.timing_points.crend()){
			point.beat_duration = std::chrono::duration_cast<std::chrono::microseconds>(
				0.01f * (-v1) * last_uninherited->beat_duration);
		}
	} else{
		point.inheritable = true;
		using namespace std::chrono_literals;
		point.beat_duration = std::chrono::duration_cast<std::chrono::microseconds>(v1 * 1ms);
	}
	parse_i(2, point.meter);
	parse_i(3, point.sample_set);
	parse_i(4, point.sample_index);
	parse_i(5, point.sample_volume);
	if(!tokens[7].empty()){
		point.kiai = tokens[7][0] == '1';
	}

	beatmap_.timing_points.push_back(point);
	return true;
}

bool osu::Beatmap_parser::parse_hitobjects(std::string_view line)
{
	const auto tokens = split(line, ',');
	if(tokens.size() < 4) return true;
	std::for_each(tokens.begin(), tokens.end(), ltrim_view);

	const auto parse_i = [&](auto i, auto& v)
	{
		std::from_chars(tokens[i].data(), tokens[i].data() + tokens[i].length(), v);
	};

	auto type = 0;
	parse_i(3, type);
	if(type & static_cast<int>(Hitobject_type::circle)){
		Hitcircle circle{};
		parse_i(0, circle.pos.x);
		parse_i(1, circle.pos.y);
		auto time = 0;
		parse_i(2, time);
		circle.time = std::chrono::milliseconds{ time };
		beatmap_.circles.push_back(circle);
	} else if(type & static_cast<int>(Hitobject_type::slider)){
		if(tokens.size() < 8) return true;
		Slider slider{};
		Point point{};
		parse_i(0, point.x);
		parse_i(1, point.y);
		slider.points.push_back(point);
		auto time = 0;
		parse_i(2, time);
		slider.time = std::chrono::milliseconds{ time };
		parse_i(6, slider.repeat);
		parse_i(7, slider.length);

		const auto sub_tokens = split(tokens[5], '|');
		if(sub_tokens.size() < 2) return true;
		std::for_each(sub_tokens.begin(), sub_tokens.end(), ltrim_view);

		const auto valid_slider_type = [&](const char slider_type)
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
		if(sub_tokens[0].empty() || !valid_slider_type(sub_tokens[0][0])) return true;

		slider.type = static_cast<Slider::Slider_type>(sub_tokens[0][0]);
		std::transform(sub_tokens.cbegin() + 1, sub_tokens.cend(), std::back_inserter(slider.points), [](auto t)
		{
			Point point{};
			const auto pos = std::from_chars(t.data(), t.data() + t.length(), point.x).ptr;
			std::from_chars(pos + 1, t.data() + t.length(), point.y);
			return point;
		});
		beatmap_.sliders.push_back(slider);

	} else if(type & static_cast<int>(Hitobject_type::spinner)){
		if(tokens.size() < 6) return true;
		Spinner spinner{};
		auto time = 0;
		parse_i(2, time);
		spinner.start = std::chrono::milliseconds{ time };
		parse_i(5, time);
		spinner.end = std::chrono::milliseconds{ time };
		beatmap_.spinners.push_back(spinner);
	}

	return true;
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

std::optional<osu::Beatmap_file> osu::Beatmap_parser::parse_impl()
{
	if(!file_.is_open()) return std::nullopt;

	using String_mod_fn = void(std::string&);
	const auto format_utf16 = maybe_parse_utfheader()
		                          ? static_cast<String_mod_fn*>([](std::string& s)
		                          {
			                          s.erase(std::remove(s.begin(), s.end(), '\0'), s.end());
		                          })
		                          : static_cast<String_mod_fn*>([](std::string&) {});

	std::string line;
	const std::string_view version_prefix = "osu file format v";

	const auto seek_version_string = [&]
	{
		while (std::getline(file_, line)) {
			format_utf16(line);
			if (const auto pos = line.find(version_prefix);
				pos != std::string::npos) {
				return pos;
			}
		}
		return std::string::npos;
	};
	
	if(const auto prefix_pos = seek_version_string();
		prefix_pos == std::string::npos){
		return std::nullopt;
	}
	else {
		const std::string_view number_string = {
			line.data() + prefix_pos + version_prefix.length(),
			line.length() - version_prefix.length() - prefix_pos
		};

		if (const auto ec = std::from_chars(number_string.data(),
			number_string.data() + number_string.length(), beatmap_.version).ec;
			ec == std::errc::invalid_argument || ec == std::errc::result_out_of_range) {
			return std::nullopt;
		}
	}

	while(std::getline(file_, line)){
		format_utf16(line);
		trim(line);

		if(line.length() == 0) continue;
		if(starts_with(line, "[")) section_ = parse_section(line);
		else if(section_ == Section::general) parse_general(line);
		else if(section_ == Section::editor) parse_editor(line);
		else if(section_ == Section::metadata) parse_metadata(line);
		else if(section_ == Section::difficulty) parse_difficulty(line);
		else if(section_ == Section::events) parse_events(line);
		else if(section_ == Section::timing_points) parse_timing_points(line);
		else if(section_ == Section::hitobjects) parse_hitobjects(line);
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
