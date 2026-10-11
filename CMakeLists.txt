cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

project(SWGEmuStructureBuilder VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enables CTest framework and creates a BUILD_TESTING option (ON by default)
include(CTest)

add_subdirectory(src)

# Only configure tests if testing is enabled
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()