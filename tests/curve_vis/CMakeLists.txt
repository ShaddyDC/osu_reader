find_package(glfw3 REQUIRED)

add_subdirectory(glm)

add_executable(curve_vis src/main.cpp src/glad.cpp)

set_target_properties(curve_vis PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        )

target_link_libraries(curve_vis glfw ${CMAKE_DL_LIBS} osuReader::osuReader glm::glm)

target_include_directories(curve_vis PRIVATE $<TARGET_PROPERTY:osuReader::osuReader,INCLUDE_DIRECTORIES>)
