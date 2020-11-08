#pragma once

#include <fstream>
#include <optional>

template<typename Provider>
class Binary_reader {};

template<>
class Binary_reader<std::ifstream>{
public:
    Binary_reader(std::ifstream& input): input{ input }{}

    template<typename Type>
    std::optional<Type> read_type();

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


template<>
class Binary_reader<std::string_view>{
public:
    Binary_reader(std::string_view input): input{ input }{}

    template<typename Type>
    std::optional<Type> read_type();

private:
    std::string_view input;
    int index = 0;
};

template<typename Type>
std::optional<Type> Binary_reader<std::string_view>::read_type()
{
    if(index + sizeof(Type) > input.size()) return std::nullopt;
    Type value = *reinterpret_cast<const Type*>(&(input.data()[index]));
    index += sizeof(Type);
    return value;
}
