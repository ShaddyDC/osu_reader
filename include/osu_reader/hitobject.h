#pragma once
#include <vector>
#include <chrono>

namespace osu{
	enum class Hitobject_type : uint_fast8_t{
		circle = 1,
		slider = 2,
		new_combo = 4,
		spinner = 8,
		mania_holdnote = 128
	};


	struct Point{
		float x, y;
	};

	struct Hitcircle{
		Point pos;
		std::chrono::milliseconds time;
	};

	struct Slider{
		enum class Slider_type : char{
			linear = 'L',
			perfect = 'P',
			bezier = 'B',
			catmull = 'C'
		};

		std::chrono::milliseconds time;
		Slider_type type;
		std::vector<Point> points;
		int repeat;
		float length;
		std::chrono::milliseconds duration;
	};

	struct Spinner{
		std::chrono::milliseconds start;
		std::chrono::milliseconds end;
	};
}
