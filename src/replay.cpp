#include <fstream>
#include "osu_reader/replay.h"
#include "binary_reader.h"
#include "replay_reader.h"

std::optional<osu::Replay> osu::Replay::from_file(const std::filesystem::path& file_path)
{
    std::ifstream file{ file_path, std::ios::binary };
    if(!file.is_open()) return std::nullopt;

    Binary_reader<std::ifstream> provider{ file };
    auto reader = Replay_reader{ provider };

    return reader.parse_replay();
}

std::optional<osu::Replay> osu::Replay::from_string(const std::string_view content)
{
    Binary_reader<std::string_view > provider{ content };
    auto reader = Replay_reader{ provider };

    return reader.parse_replay();
}
