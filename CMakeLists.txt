cmake_minimum_required(VERSION 3.12)

project(chip8)

# temporary hack
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(chip8 main.cpp chip8.cpp screen.cpp bitmapfont.cpp)

target_link_libraries(chip8 PRIVATE
    SDL2::SDL2
    ${SDL2_IMAGE_LIBRARIES})

set_target_properties(chip8 PROPERTIES
    CMAKE_CXX_STANDARD 11
    CMAKE_CXX_STANDARD_REQUIRED ON)

file(COPY res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
