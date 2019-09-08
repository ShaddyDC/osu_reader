
find_package(Catch2 REQUIRED)

add_executable(osuReaderTests src/main.cpp
	src/Kakoi-naki.cpp
	src/Nanatsu_Koyoto.cpp
	src/no_beatmap.cpp
)

target_link_libraries(osuReaderTests
	osuReader
	Catch2::Catch2
)

set_target_properties(osuReaderTests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

add_custom_command(
        TARGET osuReaderTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/res
                ${CMAKE_CURRENT_BINARY_DIR}/res
)

include(CTest)
include(Catch)

catch_discover_tests(osuReaderTests)

add_test(COMMAND osuReaderTests)
