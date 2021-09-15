#pragma once

#include "binary_reader.h"
#include "parse_string.h"
#include "string_stuff.h"
#include <optional>
#include <osu_reader/replay.h>
#include <variant>

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

        std::optional<std::variant<Binary_reader<std::ifstream>, Binary_reader<std::string_view>>> provider = {};
    };

    template<typename Type>
    bool Replay_reader::read_type(Type& value)
    {
        auto val = std::visit([](auto& p) { return p.template read_type<Type>(); }, *provider);
        if(!val) return false;
        value = val.value();
        return true;
    }

}// namespace osu
