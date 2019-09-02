#include "osu_reader/beatmap.h"
#include <charconv>
#include "string_stuff.h"
#include <cctype>

template<typename Type>
bool maybe_parse(std::string_view line, std::string_view prefix, Type& value)
{
	if(starts_with(line, prefix)){
		const auto value_string = ltrim_view({line.data() + prefix.length(), line.length() - prefix.length()});
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
		value = std::chrono::milliseconds{v};
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

tl::expected<osu::Beatmap_file, std::string> osu::Beatmap_parser::parse(const std::filesystem::path& file)
{
	return Beatmap_parser{file}.parse_impl();
}

osu::Beatmap_parser::Beatmap_parser(const std::filesystem::path& file)
	: file_{file} {}

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
	if (!starts_with(line, prefix)) {
		return false;
	}
	const auto value = ltrim_view({ line.data() + prefix.length(), line.length() - prefix.length() });
	for(const auto token : split(value, ' ')){
		const auto t = ltrim_view(token);
		int v = 0;
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
	if (const std::string_view bg_prefix = "0,0,\"",	// 0,0,"BG.png",0,0
		bg_suffix = "\",0,0";
		starts_with(line, bg_prefix)) {
		beatmap_.background = std::string{ line.cbegin() + bg_prefix.length(), line.cend() - bg_suffix.length() };
		return true;
	}
	if(const std::string_view break_prefix = "2,";	// 2,start,end
		starts_with(line, break_prefix)){
		const std::string_view value_string = { line.data() + break_prefix.length(), line.length() - break_prefix.length() };
		auto tokens = split(value_string, ',');
		if (tokens.size() != 2) return true;
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
	return true; //Todo: Implement
}

bool osu::Beatmap_parser::parse_hitobjects(std::string_view line)
{
	return true; //Todo: Implement
}

tl::expected<osu::Beatmap_file, std::string> osu::Beatmap_parser::parse_impl()
{
	if(!file_.is_open()) return tl::make_unexpected("Couldn't open file");

	std::string line;

	const std::string_view version_prefix = "osu file format v";
	if(!std::getline(file_, line) || !starts_with(ltrim(line), version_prefix)){
		return tl::make_unexpected("Couldn't parse version prefix");
	}

	std::string_view number = {
		line.data() + version_prefix.length(),
		line.length() - version_prefix.length()
	};
	if(const auto ec = std::from_chars(number.data(),
	                                   number.data() + number.length(), beatmap_.version).ec;
		ec == std::errc::invalid_argument || ec == std::errc::result_out_of_range){
		return tl::make_unexpected("Couldn't parse version number");
	}

	while(std::getline(file_, line)){
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
	if(line == "[Timing Points]") return Section::timing_points;
	if(line == "[Colours]") return Section::colours;
	if(line == "[Hit Objects]") return Section::hitobjects;
	return Section::none;
}
