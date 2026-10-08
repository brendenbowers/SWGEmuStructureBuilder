cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

project(SWGEmuStructureBuilder VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Automatically find all .cpp files in the current directory
file(GLOB_RECURSE ALL_SOURCES "src/*.cpp")

# Alternatively, if you use a 'src' folder, uncomment the line below:
# file(GLOB_RECURSE ALL_SOURCES "src/*.cpp")

# Define the final executable name and its source files
add_executable(SWGEmuStructureBuilder ${ALL_SOURCES})


# Add the include folder to the build target
# 'PRIVATE' means these headers are used to build this executable specifically.
target_include_directories(SWGEmuStructureBuilder PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

file(GLOB EXTERN_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/extern/*/include")

# 2. Loop through and add each one to your project
foreach(INCLUDE_DIR ${EXTERN_INCLUDES})
    if(IS_DIRECTORY ${INCLUDE_DIR})
        target_include_directories(SWGEmuStructureBuilder PRIVATE ${INCLUDE_DIR})
    endif()
endforeach()