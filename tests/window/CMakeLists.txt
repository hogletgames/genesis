list(APPEND GE_CORE_TEST_SRC
    events_test.cpp
    )

add_executable(genesis_window_test ${GE_CORE_TEST_SRC})
target_link_libraries(genesis_window_test PRIVATE
    genesis::window
    gtest_main
    )

gtest_discover_tests(genesis_window_test)
