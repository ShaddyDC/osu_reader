#include "string_stuff.h"
#include <locale>

std::vector<std::string_view> split(const std::string_view s, const char delim)
{
    std::vector<std::string_view> ret;
    auto it = s.begin();
    auto it_start = s.begin();

    const auto add_if_not_zero = [&] {
        if(it != it_start) {
            ret.emplace_back(s.data() + (it_start - s.begin()), it - it_start);
        }
    };

    for(; it != s.end(); ++it) {
        if(*it == delim) {
            add_if_not_zero();
            it_start = it + 1;
        }
    }
    add_if_not_zero();
    return ret;
}
