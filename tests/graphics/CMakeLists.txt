ge_add_gtest(genesis_graphics_test
    SOURCES shader_reflection_test.cpp
    LIBS genesis::graphics
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
