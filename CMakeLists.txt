cmake_minimum_required(VERSION 3.0)
project(RayGameTest LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED YES)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(WIN32_DEBUG_MEMORY_LEAKS "If set, enables memory leak debugging on Windows" OFF)

add_subdirectory(raylib)
add_subdirectory(thirdparty)
add_subdirectory(gamelib)
add_subdirectory(game)
add_subdirectory(playground)
