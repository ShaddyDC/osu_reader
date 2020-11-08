#pragma once

#include <osu_reader/replay.h>
#include <optional>
#include <lzma.h>
#include "string_stuff.h"
#include "parse_string.h"

template<typename Provider>
class Replay_reader {
public:
    Replay_reader(Provider& provider): provider{ provider }{}

    std::optional<osu::Replay> parse_replay();
    static std::optional<std::vector<osu::Replay::Replay_frame>> decode_frames(std::vector<char>& compressed);

private:
    template<typename Type>
    bool read_type(Type& value);
    bool read_type(std::string& value);
    bool read_type(Gamemode& value);
    bool read_type(std::chrono::system_clock::time_point& value);
    std::optional<int> read_uleb128();
    bool read_replaydata(std::vector<char>& value);
    static std::optional<std::string> lzma_decode(std::vector<char>& compressed);

    Provider& provider;
};

template<typename Provider>
std::optional<osu::Replay> Replay_reader<Provider>::parse_replay()
{
    osu::Replay replay;

    const auto success =
            read_type(replay.mode)
            && read_type(replay.game_version)
            && read_type(replay.map_hash)
            && read_type(replay.player_name)
            && read_type(replay.replay_hash)
            && read_type(replay.count_300)
            && read_type(replay.count_100)
            && read_type(replay.count_50)
            && read_type(replay.count_geki)
            && read_type(replay.count_katsu)
            && read_type(replay.count_miss)
            && read_type(replay.score)
            && read_type(replay.max_combo)
            && read_type(replay.full_combo)
            && read_type(replay.mods)
            && read_type(replay.life_bar)
            && read_type(replay.time_stamp)
            && read_replaydata(replay.replay_compressed)
            && read_type(replay.score_id);

    if(!success) return std::nullopt;
    return replay;
}

template<typename Provider>
template<typename Type>
bool Replay_reader<Provider>::read_type(Type& value)
{
    auto val = provider.template read_type<Type>();
    if(!val) return false;
    value = val.value();
    return true;
}

template<typename Provider>
bool Replay_reader<Provider>::read_type(std::string& value)
{
    const auto type = provider.template read_type<std::uint8_t>();
    if(type.value_or(0) == 0) {
        value = "";
        return true;
    };

    if(*type != 0x0b) return false;
    const auto length_opt = read_uleb128();
    if(!length_opt) return false;

    const auto length = *length_opt;
    value.resize(length);
    const auto bytes = provider.read_bytes(length);
    if(!bytes) return false;
    value = std::string{ bytes->begin(), bytes->end() };
    return true;
}

template<typename Provider>
std::optional<int> Replay_reader<Provider>::read_uleb128()
{
    std::uint8_t tmp = 0;

    if(!read_type(tmp)) return std::nullopt;

    std::uint8_t sum = tmp & 0x7f;

    for(int i = 1; tmp &  0x80; ++i){
        if(!read_type(tmp)) return std::nullopt;
        sum |= (tmp & 0x7f) << 7 * i;
    }
    return sum;
}

template<typename Provider>
bool Replay_reader<Provider>::read_replaydata(std::vector<char>& value)
{
    const auto compressed_size_opt =  provider.template read_type<int>();
    if(!compressed_size_opt) return false;

    const auto compressed_size = *compressed_size_opt;
    const auto data = provider.read_bytes(compressed_size);
    if(!data) return false;
    value = *data;
    return true;
}

template<typename Provider>
bool Replay_reader<Provider>::read_type(Gamemode& value)
{
    const auto value_opt = provider.template read_type<std::uint8_t>();
    if(!value_opt) return false;
    value = static_cast<Gamemode>(*value_opt);
    return true;
}

template<typename Provider>
bool Replay_reader<Provider>::read_type(std::chrono::system_clock::time_point& value)
{
    const auto value_opt = provider.template read_type<std::uint64_t>();
    if(!value_opt) return false;
    value = std::chrono::time_point<std::chrono::system_clock>{ std::chrono::nanoseconds{ *value_opt * 100 } };
    return true;
}

template<typename Provider>
std::optional<std::string> Replay_reader<Provider>::lzma_decode(std::vector<char>& compressed)
{
    if(compressed.empty()) return std::nullopt;

    constexpr const auto buff_size = 4096;
    char buffer[buff_size];
    std::string output;

    lzma_stream strm = LZMA_STREAM_INIT;
    strm.next_in = reinterpret_cast<const std::uint8_t*>(compressed.data());
    strm.avail_in = compressed.size();

    if(lzma_auto_decoder(&strm, UINT64_MAX, LZMA_CHECK_CRC64) != LZMA_OK){
        return std::nullopt;
    }

    lzma_ret res;
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
}

template<typename Provider>
std::optional<std::vector<osu::Replay::Replay_frame>>
Replay_reader<Provider>::decode_frames(std::vector<char>& compressed)
{
    const auto str_opt = lzma_decode(compressed);
    if(!str_opt) return std::nullopt;

    const auto str = *str_opt;
    const auto lines = split(str, ',');
    std::vector<osu::Replay::Replay_frame> frames;
    frames.reserve(lines.size());

    auto current_time = 0;
    for(const auto line : lines){
        const auto tokens = split(line, '|');
        if(tokens.size() == 0) continue;
        if(tokens.size() != 4) return std::nullopt;
        current_time += parse_value<int>(tokens[0]);
        frames.push_back({
                                 current_time,
                                 parse_value<float>(tokens[1]),
                                 parse_value<float>(tokens[2]),
                                 parse_value<int>(tokens[3]),
                         });
    }
    return frames;
}
