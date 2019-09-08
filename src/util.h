#pragma once

namespace util{
	inline char operator "" _ch(const unsigned long long int value)
	{
		return static_cast<char>(value);
	}
}
