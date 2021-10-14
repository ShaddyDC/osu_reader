
find_package(Catch2 QUIET)

if (NOT Catch2_FOUND)
    add_subdirectory(Catch2 EXCLUDE_FROM_ALL)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Catch2/contrib)
endif (NOT Catch2_FOUND)


add_executable(osuReaderTests src/main.cpp
        src/beatmap_Kakoi-naki.cpp
        src/beatmap_Nanatsu_Koyoto.cpp
        src/parse_all.cpp
        src/file_formats.cpp
        src/beatmap_util.cpp
        src/timingpoints.cpp
        src/string_stuff.cpp
        src/replay_cptnXn_fdfd.cpp
        src/slider_parsing.cpp
        src/beatmap_hitobj_it.cpp
        )

target_link_libraries(osuReaderTests
        osuReader::osuReader
        Catch2::Catch2
        )

set_target_properties(osuReaderTests PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        )

target_include_directories(osuReaderTests
        PRIVATE $<TARGET_PROPERTY:osuReader::osuReader,INCLUDE_DIRECTORIES>
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

# Visualisers
if (ENABLE_TEST_CURVE_VIS)
    add_subdirectory(curve_vis)
endif ()
