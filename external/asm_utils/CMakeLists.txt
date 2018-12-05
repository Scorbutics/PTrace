cmake_minimum_required(VERSION 3.9)
project(asm_utils)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES
    src/*.h
    src/*.cpp
)

string(REGEX REPLACE "[^;]*CMakeFiles/[^;]+;?" "" SOURCES "${SOURCES}")

add_library(asm_utils SHARED ${SOURCES})


