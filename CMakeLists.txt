cmake_minimum_required(VERSION 3.12)

project(chip8)

find_package(SDL2 REQUIRED)

add_executable(chip8 main.cpp chip8.cpp screen.cpp)

target_link_libraries(chip8 PRIVATE
    SDL2::SDL2)

set_target_properties(chip8 PROPERTIES
    CMAKE_CXX_STANDARD 11
    CMAKE_CXX_STANDARD_REQUIRED ON)
