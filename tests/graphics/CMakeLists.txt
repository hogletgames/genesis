list(APPEND GE_GRAPHICS_TEST_SRC
    shader_reflection_test.cpp
    )

add_executable(genesis_graphics_test ${GE_GRAPHICS_TEST_SRC})
target_link_libraries(genesis_graphics_test PRIVATE
    genesis::graphics
    gtest_main
    )

gtest_discover_tests(genesis_graphics_test
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )
