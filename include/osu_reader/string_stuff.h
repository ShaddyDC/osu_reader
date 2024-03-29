#pragma once
#include <algorithm>
#include <cctype>
#include <locale>
#include <string_view>
#include <vector>

namespace osu {
    inline bool starts_with(std::string_view s, std::string_view prefix)
    {
        return s.rfind(prefix, 0) == 0;
    }

    // Trim: https://stackoverflow.com/a/217605
    // trim from start (in place)
    inline std::string& ltrim(std::string& s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](const int ch) {
                    return std::isspace(static_cast<unsigned char>(ch)) == 0;
                }));
        return s;
    }

    // trim from end (in place)
    inline std::string& rtrim(std::string& s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](const int ch) {
                    return std::isspace(static_cast<unsigned char>(ch)) == 0;
                }).base(),
                s.end());
        return s;
    }

    // trim from both ends (in place)
    inline std::string& trim(std::string& s)
    {
        ltrim(s);
        rtrim(s);
        return s;
    }

    // trim from start (copying)
    inline std::string ltrim_copy(std::string s)
    {
        ltrim(s);
        return s;
    }

    // trim from end (copying)
    inline std::string rtrim_copy(std::string s)
    {
        rtrim(s);
        return s;
    }

    // trim from both ends (copying)
    inline std::string trim_copy(std::string s)
    {
        trim(s);
        return s;
    }

    inline std::string_view ltrim_view(const std::string_view s)
    {
        const auto* const it = std::find_if(s.begin(), s.end(), [](const int ch) {
            return std::isspace(static_cast<unsigned char>(ch)) == 0;
        });
        return std::string_view{
                s.data() + std::distance(s.begin(), it),
                static_cast<std::size_t>(std::distance(it, s.end()))};
    }

    inline std::string_view rtrim_view(const std::string_view s)
    {
        const auto it = std::find_if(s.rbegin(), s.rend(), [](const int ch) {
            return std::isspace(static_cast<unsigned char>(ch)) == 0;
        });
        return std::string_view{
                s.data(),
                static_cast<std::size_t>(s.size() - std::distance(s.rbegin(), it))};
    }

    inline std::string_view trim_view(const std::string_view s)
    {
        return ltrim_view(rtrim_view(s));
    }

    std::vector<std::string_view> split(std::string_view s, char delim);
}// namespace osu
