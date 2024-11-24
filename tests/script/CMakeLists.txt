include(${PROJECT_SOURCE_DIR}/cmake/GeMonoHelpers.cmake)
ge_mono_create_assembly(GeScriptTests.dll
    SOURCES
        data/MethodTests.cs
    )

list(APPEND SCRIPT_TEST_SRC
    main.cpp
    method_test.cpp
    )

add_executable(genesis_script_test ${SCRIPT_TEST_SRC})
target_include_directories(genesis_script_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(genesis_script_test PRIVATE
    genesis::filesystem
    genesis::script
    gmock
    gtest
    )
add_dependencies(genesis_script_test GeScriptTests.dll)

gtest_discover_tests(genesis_script_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

