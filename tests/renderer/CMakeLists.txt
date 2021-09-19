list(APPEND GE_RENDERER_TEST_SRC
    shader_reflection_test.cpp)

add_executable(genesis_renderer_test ${GE_RENDERER_TEST_SRC})
target_link_libraries(genesis_renderer_test PRIVATE
    genesis::renderer
    gtest_main)

add_test(NAME genesis_renderer_test
    COMMAND genesis_renderer_test
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
