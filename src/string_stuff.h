#pragma once
#include <string_view>

bool starts_with(std::string_view s, std::string_view prefix);


// trim from start (in place)
std::string& ltrim(std::string& s);

// trim from end (in place)
std::string& rtrim(std::string& s);

// trim from both ends (in place)
std::string& trim(std::string& s);

// trim from start (copying)
std::string ltrim_copy(std::string s);

// trim from end (copying)
std::string rtrim_copy(std::string s);

// trim from both ends (copying)
std::string trim_copy(std::string s);

std::string_view ltrim_view(std::string_view s);
