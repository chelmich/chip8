cmake_minimum_required(VERSION 3.12)

project(chip8 LANGUAGES CXX)

find_package(SDL2 REQUIRED)
find_package(sdl2-image REQUIRED)

add_executable(${PROJECT_NAME})

set_target_properties(chip8
    PROPERTIES
        CMAKE_CXX_STANDARD 11
        CMAKE_CXX_STANDARD_REQUIRED ON
)

target_sources(${PROJECT_NAME}
    PRIVATE
        main.cpp
        chip8.cpp
        screen.cpp
        bitmapfont.cpp
)

target_link_libraries(chip8
    PRIVATE
        SDL2::SDL2
        SDL2::SDL2main
        SDL2::SDL2_image
)

file(COPY res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
