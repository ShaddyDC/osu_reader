#include "string_stuff.h"
#include <algorithm>
#include <cctype>
#include <locale>

bool starts_with(std::string_view s, std::string_view prefix)
{
	return s.rfind(prefix, 0) == 0;
}


// Trim: https://stackoverflow.com/a/217605
// trim from start (in place)
std::string& ltrim(std::string& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch)
	{
		return !std::isspace(static_cast<unsigned char>(ch));
	}));
	return s;
}

// trim from end (in place)
std::string& rtrim(std::string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch)
	{
		return !std::isspace(static_cast<unsigned char>(ch));
	}).base(), s.end());
	return s;
}

// trim from both ends (in place)
std::string& trim(std::string& s)
{
	ltrim(s);
	rtrim(s);
	return s;
}

// trim from start (copying)
std::string ltrim_copy(std::string s)
{
	ltrim(s);
	return s;
}

// trim from end (copying)
std::string rtrim_copy(std::string s)
{
	rtrim(s);
	return s;
}

// trim from both ends (copying)
std::string trim_copy(std::string s)
{
	trim(s);
	return s;
}

std::string_view ltrim_view(const std::string_view s)
{
	const auto it = std::find_if(s.begin(), s.end(), [](int ch)
	{
		return !std::isspace(static_cast<unsigned char>(ch));
	});
	return std::string_view{s.data() + (it - s.begin()), static_cast<std::size_t>(s.end() - it)};
}
