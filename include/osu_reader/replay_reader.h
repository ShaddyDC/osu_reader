#pragma once

#include "binary_reader.h"
#include "parse_string.h"
#include "string_stuff.h"
#include <optional>
#include <osu_reader/replay.h>
#include <variant>

// TODO: Don't import LZMA in user code
#ifdef ENABLE_LZMA
#include <lzma.h>
#endif

namespace osu {
    class Replay_reader {
    public:
        std::optional<osu::Replay> from_file(const std::filesystem::path& file_path, bool parse_frames);
        std::optional<osu::Replay> from_string(const std::string_view content, bool parse_frames);

        std::optional<Replay> parse_replay();

    private:
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
