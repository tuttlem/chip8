cmake_minimum_required(VERSION 3.24)
project(chip8 C)

find_package(SDL2 REQUIRED)
find_package(Curses REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(CMAKE_C_STANDARD 11)

add_executable(chip8 main.c machine.h machine.c machine_exec.c interface.h interface.c)
target_link_libraries(chip8 ${SDL2_LIBRARIES})
target_link_libraries(chip8 ${CURSES_LIBRARIES})