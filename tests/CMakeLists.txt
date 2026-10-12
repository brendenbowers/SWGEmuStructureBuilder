enable_testing()

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.16.1
)

FetchContent_MakeAvailable(Catch2)

file(GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp")

add_executable(SWGEmuStructureBuilderTest ${ALL_SOURCES})

target_link_libraries(SWGEmuStructureBuilderTest PRIVATE Catch2::Catch2WithMain)
target_link_libraries(SWGEmuStructureBuilderTest PRIVATE SWGEmuStructureBuilderLib)

# Catch2 discover tests
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(SWGEmuStructureBuilderTest)

# Register the executable with CTest so it gets executed automatically
#add_test(NAME StructureBuilderTests COMMAND SWGEmuStructureBuilderTest)