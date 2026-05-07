cmake_minimum_required(VERSION 3.16)
project(chip8-emulator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

add_executable(chip8
    src/main.cpp
    src/chip8.cpp
    src/platform.cpp
)

target_include_directories(chip8 PRIVATE src)
target_link_libraries(chip8 sfml-graphics sfml-window sfml-system sfml-audio)
