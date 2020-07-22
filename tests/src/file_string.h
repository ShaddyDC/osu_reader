#pragma once
#include <fstream>
#include <string>

inline std::string file_string(const char* filename)
{    
    std::ifstream f(filename);
    return std::string((std::istreambuf_iterator<char>(f)),
                    std::istreambuf_iterator<char>());
}
