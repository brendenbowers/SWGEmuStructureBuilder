# Automatically find all .cpp files in the current directory
file(GLOB_RECURSE ALL_SOURCES "*.cpp")

add_library(SWGEmuStructureBuilderLib ${ALL_SOURCES})

get_filename_component(ROOT_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

# Add the include folder to the build target
# 'PRIVATE' means these headers are used to build this executable specifically.
target_include_directories(SWGEmuStructureBuilderLib PUBLIC
    ROOT_FOLDER/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)



file(GLOB EXTERN_INCLUDES "${ROOT_FOLDER}/extern/*/include")
# 2. Loop through and add each one to your project
foreach(INCLUDE_DIR ${EXTERN_INCLUDES})
    if(IS_DIRECTORY ${INCLUDE_DIR})
        message(STATUS "INCLUDE_DIR: ${INCLUDE_DIR}")
        target_include_directories(SWGEmuStructureBuilderLib PUBLIC ${INCLUDE_DIR})
    endif()
endforeach()


# Define the final executable name and its source files
add_executable(SWGEmuStructureBuilder main.cpp)
target_link_libraries(SWGEmuStructureBuilder PRIVATE SWGEmuStructureBuilderLib)