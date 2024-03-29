#include "osu_reader/replay_reader.h"
#include "binary_reader.h"
#include "osu_reader/string_stuff.h"
#include "parse_string.h"

#ifdef ENABLE_LZMA
#include <lzma.h>
#endif

std::optional<osu::Replay> osu::Replay_reader::from_file(const std::filesystem::path& file_path)
{
    std::ifstream file{file_path, std::ios::binary};
    if(!file.is_open()) return std::nullopt;

    provider = std::make_unique<Binary_file_reader>(file);

    return parse_replay();
}

std::optional<osu::Replay> osu::Replay_reader::from_string(std::string_view content)
{
    provider = std::make_unique<Binary_string_reader>(content);

    return parse_replay();
}
std::optional<osu::Replay> osu::Replay_reader::parse_replay()
{
    Replay replay;

    const auto success =
            read_type(replay.mode) && read_type(replay.game_version) && read_type(replay.map_hash) && read_type(replay.player_name) && read_type(replay.replay_hash) && read_type(replay.count_300) && read_type(replay.count_100) && read_type(replay.count_50) && read_type(replay.count_geki) && read_type(replay.count_katsu) && read_type(replay.count_miss) && read_type(replay.score) && read_type(replay.max_combo) && read_type(replay.full_combo) && read_type(replay.mods) && read_type(replay.life_bar) && read_type(replay.time_stamp) && read_replaydata(replay.replay_compressed) && read_type(replay.score_id);

    if(!success) return std::nullopt;

    if(parse_frames) {
        replay.frames = decode_frames(replay.replay_compressed);
    }

    return replay;
}

bool osu::Replay_reader::read_type(std::string& value)
{
    uint8_t type;
    const auto res = read_type(type);
    if(!res or type == 0) {
        value = "";
        return true;
    }

    if(type != 0x0b) return false;
    const auto length_opt = read_uleb128();
    if(!length_opt) return false;

    const auto length = *length_opt;
    //        value.resize(length);
    const auto bytes = provider->read_bytes(length);
    if(!bytes) return false;
    value = std::string{bytes->begin(), bytes->end()};
    return true;
}

std::optional<int> osu::Replay_reader::read_uleb128()
{
    uint8_t tmp = 0;

    if(!read_type(tmp)) return std::nullopt;

    uint8_t sum = tmp & 0x7f;

    for(int i = 1; (tmp & 0x80) != 0; ++i) {
        if(!read_type(tmp)) return std::nullopt;
        sum |= (tmp & 0x7f) << 7 * i;
    }
    return sum;
}

bool osu::Replay_reader::read_replaydata(std::vector<char>& value)
{
    int32_t compressed_size = 0;
    const auto success = read_type(compressed_size);
    if(!success) return false;

    const auto data = provider->read_bytes(compressed_size);
    if(!data) return false;
    value = *data;
    return true;
}

bool osu::Replay_reader::read_type(Gamemode& value)
{
    std::underlying_type<Gamemode>::type v = {};
    const auto success = read_type(v);
    if(!success) return false;
    value = static_cast<Gamemode>(v);
    return true;
}

bool osu::Replay_reader::read_type(std::chrono::time_point<std::chrono::nanoseconds>& value)
{
    using Ticks = std::chrono::duration<int64_t,
                                        std::ratio_multiply<std::ratio<100>, std::nano>>;

    uint64_t v = {};
    const auto success = read_type(v);

    if(!success) return false;

    // (sys_days{1970_y/jan/1} - sys_days{0001_y/jan/1} = 621355968000000000
    value = std::chrono::time_point<std::chrono::nanoseconds>{Ticks{v - 621355968000000000}};
    return true;
}
std::optional<std::string> osu::Replay_reader::lzma_decode(std::vector<char>& compressed)
{
#ifdef ENABLE_LZMA
    if(compressed.empty()) return std::nullopt;

    constexpr const auto buff_size = 4096;
    char buffer[buff_size];
    std::string output;

    lzma_stream strm = LZMA_STREAM_INIT;
    strm.next_in = reinterpret_cast<const uint8_t*>(compressed.data());
    strm.avail_in = compressed.size();

    if(lzma_auto_decoder(&strm, UINT64_MAX, LZMA_CHECK_CRC64) != LZMA_OK) {
        return std::nullopt;
    }

    lzma_ret res{};
    do {
        strm.next_out = reinterpret_cast<uint8_t*>(buffer);
        strm.avail_out = buff_size;
        res = lzma_code(&strm, LZMA_FINISH);
        if(strm.avail_out != buff_size) {
            std::copy(std::begin(buffer), std::end(buffer) - strm.avail_out, std::back_inserter(output));
        }
    } while(res == LZMA_OK || res == LZMA_GET_CHECK);

    if(res != LZMA_STREAM_END) return std::nullopt;

    return output;
#else
    throw std::runtime_error{"Trying to decode replay frames, but compiled with option ENABLE_LZMA=OFF"};
#endif
}

std::optional<std::vector<osu::Replay::Replay_frame>>
osu::Replay_reader::decode_frames(std::vector<char>& compressed)
{
#ifdef ENABLE_LZMA
    const auto str_opt = lzma_decode(compressed);
    if(!str_opt) return std::nullopt;

    const auto str = *str_opt;
    const auto lines = split(str, ',');
    std::vector<Replay::Replay_frame> frames;
    frames.reserve(lines.size());

    auto current_time = 0;
    for(const auto line : lines) {
        const auto tokens = split(line, '|');
        if(tokens.empty()) continue;
        if(tokens.size() != 4) return std::nullopt;
        current_time += parse_value<int>(tokens[0]);
        frames.push_back({
                std::chrono::milliseconds{current_time},
                parse_value<float>(tokens[1]),
                parse_value<float>(tokens[2]),
                parse_value<int>(tokens[3]),
        });
    }
    return frames;
#else
    throw std::runtime_error{"Trying to decode replay frames, but compiled with option ENABLE_LZMA=OFF"};
#endif
}