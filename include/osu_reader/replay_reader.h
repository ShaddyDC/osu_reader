#pragma once

#include "binary_reader_interface.h"
#include <memory>
#include <optional>
#include <osu_reader/replay.h>

namespace osu {
    class Replay_reader {
    public:
        std::optional<osu::Replay> from_file(const std::filesystem::path& file_path);
        std::optional<osu::Replay> from_string(const std::string_view content);

        ///  Determines if replay frames should be parsed. Requires xz to decompress lzma
        bool parse_frames = false;

    private:
        std::optional<Replay> parse_replay();

        template<typename Type>
        bool read_type(Type& value);
        bool read_type(std::string& value);
        bool read_type(Gamemode& value);
        bool read_type(std::chrono::time_point<std::chrono::nanoseconds>& value);
        std::optional<int> read_uleb128();
        bool read_replaydata(std::vector<char>& value);
        static std::optional<std::string> lzma_decode(std::vector<char>& compressed);
        static std::optional<std::vector<Replay::Replay_frame>> decode_frames(std::vector<char>& compressed);

        std::unique_ptr<IBinary_reader> provider = nullptr;
    };

    template<typename Type>
    bool Replay_reader::read_type(Type& value)
    {
        if(!provider) return false;

        return provider->read_bytes(reinterpret_cast<char*>(&value), sizeof(value));
    }

}// namespace osu
