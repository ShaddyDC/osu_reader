#pragma once

#include <osu_reader/replay.h>
#include <optional>

template<typename Provider>
class Replay_reader {
public:
    Replay_reader(Provider& provider): provider{ provider }{}

    std::optional<osu::Replay> parse_replay();

private:
    template<typename Type>
    bool read_type(Type& value);
    bool read_type(std::string& value);
    bool read_type(Gamemode& value);
    bool read_type(std::chrono::system_clock::time_point& value);
    std::optional<int> read_uleb128();
    bool read_replaydata(std::vector<char>& value);

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


