#pragma once

#include <fstream>
#include <optional>
#include <vector>

template<typename Provider>
class Binary_reader {
};

template<>
class Binary_reader<std::ifstream> {
public:
    explicit Binary_reader(std::ifstream& input) : input{input} {}

    template<typename Type>
    [[nodiscard]] std::optional<Type> read_type();
    [[nodiscard]] std::optional<std::vector<char>> read_bytes(const int num);

private:
    std::ifstream& input;
};

template<typename Type>
std::optional<Type> Binary_reader<std::ifstream>::read_type()
{
    Type value;
    if(input.read(reinterpret_cast<char*>(&value), sizeof(value)))
        return value;
    return std::nullopt;
}

std::optional<std::vector<char>> Binary_reader<std::ifstream>::read_bytes(const int num)
{
    std::vector<char> buffer(num);
    if(input.read(buffer.data(), num))
        return buffer;
    return std::nullopt;
}

template<>
class Binary_reader<std::string_view> {
public:
    explicit Binary_reader(std::string_view input) : input{input} {}

    template<typename Type>
    [[nodiscard]] std::optional<Type> read_type();
    [[nodiscard]] std::optional<std::vector<char>> read_bytes(const int num);

private:
    std::string_view input;
    int index = 0;
};

template<typename Type>
std::optional<Type> Binary_reader<std::string_view>::read_type()
{
    if(index + sizeof(Type) > input.size()) return std::nullopt;
    Type value = *reinterpret_cast<const Type*>(&(input[index]));
    index += sizeof(Type);
    return value;
}

std::optional<std::vector<char>> Binary_reader<std::string_view>::read_bytes(const int num)
{
    if(index + num > static_cast<int>(input.size())) return std::nullopt;
    std::vector<char> buffer{input.begin() + index, input.begin() + index + num};
    index += num;
    return buffer;
}
