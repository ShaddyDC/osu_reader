#pragma once

#include <fstream>
#include <osu_reader/binary_reader_interface.h>

class Binary_file_reader : public osu::IBinary_reader {
public:
    explicit Binary_file_reader(std::ifstream& input) : input{input} {}

    inline bool read_bytes(char* buffer, int size) override
    {
        return static_cast<bool>(input.read(buffer, size));
    }

private:
    std::ifstream& input;
};

class Binary_string_reader : public osu::IBinary_reader {
public:
    explicit Binary_string_reader(std::string_view input) : input{input} {}

    inline bool read_bytes(char* buffer, int size) override
    {
        if(pos + size > static_cast<int>(input.size())) return false;

        std::copy(input.begin() + pos, input.begin() + pos + size, buffer);
        pos += size;
        return true;
    }

private:
    std::string_view input;
    int pos = 0;
};