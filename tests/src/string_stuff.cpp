#include <catch2/catch.hpp>
#include <osu_reader/string_stuff.h>

TEST_CASE("starts_with", "[string]")
{
    REQUIRE(osu::starts_with("hello world", "hello"));
    REQUIRE(osu::starts_with(std::string{"hello world"}, "hello"));
    REQUIRE_FALSE(osu::starts_with("hello world", "world"));
}

TEST_CASE("ltrim", "[string]")
{
    std::string s = "  hello world  ";
    REQUIRE(osu::ltrim(s) == "hello world  ");
    REQUIRE(s == "hello world  ");
}

TEST_CASE("rtrim", "[string]")
{
    std::string s = "  hello world  ";
    REQUIRE(osu::rtrim(s) == "  hello world");
    REQUIRE(s == "  hello world");
}

TEST_CASE("trim", "[string]")
{
    std::string s = "  hello world  ";
    REQUIRE(osu::trim(s) == "hello world");
    REQUIRE(s == "hello world");
}

TEST_CASE("ltrim_copy", "[string]")
{
    std::string s = "  hello world  ";
    REQUIRE(osu::ltrim_copy(s) == "hello world  ");
    REQUIRE(s == "  hello world  ");
}

TEST_CASE("rtrim_copy", "[string]")
{
    std::string s = "  hello world  ";
    REQUIRE(osu::rtrim_copy(s) == "  hello world");
    REQUIRE(s == "  hello world  ");
}

TEST_CASE("trim_copy", "[string]")
{
    std::string s = "  hello world  ";
    REQUIRE(osu::trim_copy(s) == "hello world");
    REQUIRE(s == "  hello world  ");
}

TEST_CASE("ltrim_view", "[string]")
{
    std::string s = "  hello world  ";
    REQUIRE(osu::ltrim_view(s) == "hello world  ");
    REQUIRE(s == "  hello world  ");
}

TEST_CASE("rtrim_view", "[string]")
{
    std::string s = "  hello world  ";
    REQUIRE(osu::rtrim_view(s) == "  hello world");
    REQUIRE(s == "  hello world  ");
}

TEST_CASE("trim_view", "[string]")
{
    std::string s = "  hello world  ";
    REQUIRE(osu::trim_view(s) == "hello world");
    REQUIRE(s == "  hello world  ");
}

TEST_CASE("split string", "[string]")
{
    const auto s = " The quick  brown fox jumps over  the lazy dog  , ";
    const auto splits = osu::split(s, ' ');

    REQUIRE(splits.size() == 10);
    REQUIRE(splits[0] == "The");
    REQUIRE(splits[1] == "quick");
    REQUIRE(splits[2] == "brown");
    REQUIRE(splits[3] == "fox");
    REQUIRE(splits[4] == "jumps");
    REQUIRE(splits[5] == "over");
    REQUIRE(splits[6] == "the");
    REQUIRE(splits[7] == "lazy");
    REQUIRE(splits[8] == "dog");
    REQUIRE(splits[9] == ",");
}
