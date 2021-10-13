#pragma once

#include <fstream>
#include <optional>
#include <vector>

namespace osu {
    class IBinary_reader {
    public:
        virtual bool read_bytes(char* buffer, int size) = 0;

        std::optional<std::vector<char>> read_bytes(int size)
        {
            std::vector<char> buffer(size);
            if(read_bytes(buffer.data(), size))
                return buffer;
            return std::nullopt;
        }
    };

    class Binary_file_reader : public IBinary_reader {
    public:
        explicit Binary_file_reader(std::ifstream& input) : input{input} {}

        bool read_bytes(char* buffer, int size) override
        {
            return static_cast<bool>(input.read(buffer, size));
        }

    private:
        std::ifstream& input;
    };

    class Binary_string_reader : public IBinary_reader {
    public:
        explicit Binary_string_reader(std::string_view input) : input{input} {}

        bool read_bytes(char* buffer, int size) override
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
}// namespace osu
