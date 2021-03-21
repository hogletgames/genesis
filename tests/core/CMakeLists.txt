list(APPEND GE_CORE_TEST_SRC
    timestamp_test.cpp)

add_executable(genesis_core_test ${GE_CORE_TEST_SRC})
target_link_libraries(genesis_core_test PRIVATE
    genesis::core
    gtest_main)

add_test(NAME genesis_core_test COMMAND genesis_core_test)
