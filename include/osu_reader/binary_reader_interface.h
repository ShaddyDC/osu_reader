#pragma once

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

        virtual ~IBinary_reader() = default;
    };
}// namespace osu
