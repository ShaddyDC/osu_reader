#pragma once

#include <algorithm>
#include <chrono>

namespace osu{
	template<typename Bidirectional_iterator, typename Sentinel>
	Bidirectional_iterator next_timingpoint(const Bidirectional_iterator first, const Sentinel last, const std::chrono::milliseconds time)
	{
		const auto it = std::find_if_not(first, last, 
			[time](const auto& tp)
			{
				return tp.time <= time;
			});
		if(it != first) return std::prev(it);
		return it;
	}

}
