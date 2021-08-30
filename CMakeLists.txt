cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
message(Cmake Version: ${CMAKE_VERSION})

project(osuReader VERSION 0.1.1 LANGUAGES CXX)

option(ENABLE_PYBIND "Compile python bindings" FALSE)
option(ENABLE_LZMA "Enable parsing of replay frames through xz library" TRUE)
option(ENABLE_TESTING "Enable Test Builds" ON)
option(ENABLE_TEST_CURVE_VIS "Programs to visualise curves (requires ENABLE_TESTING)" OFF)


# Replay LZMA parsing with library
if (ENABLE_LZMA)
    add_subdirectory(libs/xz)

    # Pybind requires Position Independent Code in this library
    # Untested on compilers besides gcc
    if (ENABLE_PYBIND)
        target_compile_options(liblzma PRIVATE
                $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-fPIC>
                )
    endif (ENABLE_PYBIND)
    add_compile_definitions(ENABLE_LZMA)
endif (ENABLE_LZMA)

# Sources
set(Shosu_SOURCES
        src/beatmap.cpp
        src/beatmap_parser.cpp
        src/string_stuff.cpp
        src/replay.cpp
        src/parse_hitobject.cpp
        src/sliderpath.cpp)

add_library(osuReader ${Shosu_SOURCES})

function(target_configuration target)
    target_compile_options(${target} PRIVATE
            $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -Wall -Wextra -pedantic>
            $<$<CXX_COMPILER_ID:MSVC>:
            /W4>)

    set_target_properties(${target} PROPERTIES
            CXX_STANDARD 17
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
            )

    target_include_directories(${target}
            PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            SYSTEM INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
            )

    if (ENABLE_LZMA)
        target_link_libraries(${target} PRIVATE liblzma)

        # We need to add the include directory manually because otherwise emscripten somehow doesn't pick it up
        target_include_directories(${target} PRIVATE libs/xz/src/liblzma/api)
    endif (ENABLE_LZMA)
endfunction()

target_configuration(osuReader)

# Pybind support
if (ENABLE_PYBIND)
    add_subdirectory(libs/pybind11)
    pybind11_add_module(pyshosu ${Shosu_SOURCES} src/python_bindings.cpp)
    target_configuration(pyshosu)
endif (ENABLE_PYBIND)

add_library(osuReader::osuReader ALIAS osuReader)

install(TARGETS osuReader EXPORT osuReaderTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )
install(EXPORT osuReaderTargets
        FILE osuReaderTargets.cmake
        NAMESPACE osuReader::
        DESTINATION lib/cmake/osuReader
        )

if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif (ENABLE_TESTING)

