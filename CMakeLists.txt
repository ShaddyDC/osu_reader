cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
message(Cmake Version: ${CMAKE_VERSION})

project(osuReader VERSION 0.1.1 LANGUAGES CXX)

add_library(osuReader 
	src/beatmap.cpp
	src/beatmap_parser.cpp
	src/string_stuff.cpp
)

target_compile_options(osuReader PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -Wall -Wextra -pedantic>
        $<$<CXX_COMPILER_ID:MSVC>:
            /W4>)

set_target_properties(osuReader PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

add_library(osuReader::osuReader ALIAS osuReader)

target_include_directories(osuReader 
    PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	SYSTEM INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
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


option(ENABLE_TESTING "Enable Test Builds" ON)

if(ENABLE_TESTING)	
	enable_testing()
	add_subdirectory(tests)
endif(ENABLE_TESTING)

