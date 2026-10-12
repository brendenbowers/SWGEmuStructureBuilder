# Automatically find all .cpp files in the current directory
file(GLOB_RECURSE ALL_SOURCES "*.cpp")

add_library(SWGEmuStructureBuilderLib ${ALL_SOURCES})

# Add the include folder to the build target
# 'PRIVATE' means these headers are used to build this executable specifically.
target_include_directories(SWGEmuStructureBuilderLib PUBLIC
    ${CMAKE_SOURCE_DIR}/include
     ${CMAKE_SOURCE_DIR}/src
)

file(GLOB EXTERN_INCLUDES "${CMAKE_SOURCE_DIR}/extern/*/include")
# 2. Loop through and add each one to your project
foreach(INCLUDE_DIR ${EXTERN_INCLUDES})
    if(IS_DIRECTORY ${INCLUDE_DIR})
        target_include_directories(SWGEmuStructureBuilderLib PUBLIC ${INCLUDE_DIR})
    endif()
endforeach()


include(FetchContent)
# add argparse
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG v3.2
)

FetchContent_MakeAvailable(argparse)
target_link_libraries(SWGEmuStructureBuilderLib PUBLIC argparse)


# Define the final executable name and its source files
add_executable(SWGEmuStructureBuilder main.cpp)
target_link_libraries(SWGEmuStructureBuilder PRIVATE SWGEmuStructureBuilderLib)