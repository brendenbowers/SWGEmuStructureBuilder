add_executable(SWGEmuStructureBuilderTest main.cpp)

target_link_libraries(SWGEmuStructureBuilderTest PRIVATE SWGEmuStructureBuilderLib)

# Register the executable with CTest so it gets executed automatically
add_test(NAME StructureBuilderTests COMMAND SWGEmuStructureBuilderTest)