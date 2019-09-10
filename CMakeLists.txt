cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
message(Cmake Version: ${CMAKE_VERSION})

project(osuReader VERSION 0.0.0 LANGUAGES CXX)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic)
endif()

add_library(osuReader 
	src/beatmap.cpp
	src/string_stuff.cpp
)

set_target_properties(osuReader PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(osuReader 
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
    PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
)

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

enable_testing()
add_subdirectory(tests)
