#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <cstdint>
#include <optional>
#include "gamemode.h"
#include "hitobject.h"

namespace osu
{
	struct Beatmap_file
	{

		struct Timing_point
		{
			std::chrono::milliseconds time;
			std::chrono::microseconds beat_duration;
			int meter;
			int sample_set;
			int sample_index;
			int sample_volume;
			bool kiai;
		};
			   		 		
		int version;

		// General
		std::filesystem::path audio_file;
		std::chrono::milliseconds audio_lead_in;
		std::chrono::milliseconds preview_time;
		uint_fast8_t countdown;
		std::string sample_set;
		float stack_leniency;
		Gamemode mode;
		bool letterbox_in_breaks;
		bool story_fire_in_front;
		std::string skin_preference;
		bool epilepsy_warning;
		int countdown_offset;
		bool widescreen_storyboard;
		bool special_style;
		bool use_skin_sprites;

		// Editor
		std::vector<std::chrono::milliseconds> bookmarks;
		float distance_spacing;
		int beat_divisor;
		int grid_size;
		float timeline_zoom;

		// Metadata
		std::string title;
		std::string title_unicode;
		std::string artist;
		std::string artist_unicode;
		std::string creator;
		std::string difficulty_name;
		std::string source;
		std::vector<std::string> tags;
		int beatmap_id;
		int beatmap_set_id;

		// Difficulty
		float hp;
		float cs;
		float od;
		float ar;
		float slider_multiplier = 1.4f;
		float slider_tick_rate = 1.f;

		// Events
		std::filesystem::path background;
		std::vector<std::pair<std::chrono::milliseconds, std::chrono::milliseconds>> breaks;

		// Timing Points
		std::vector<Timing_point> timing_points;

		// Hitobjects
		std::vector<Hitcircle> circles;
		std::vector<Slider> sliders;
		std::vector<Spinner> spinners;
		
	};
}
