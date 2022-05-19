cmake_minimum_required(VERSION 3.0)
project(RayGameTest LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory(raylib)
add_subdirectory(gamelib)
add_subdirectory(game)
add_subdirectory(playground)
