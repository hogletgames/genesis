ge_add_gtest(genesis_core_test
    SOURCES
        timestamp_test.cpp
        shared_library_test.cpp
    LIBS
        genesis::core
        genesis::filesystem
    DEPS ge_shared_library_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

add_subdirectory(shared_library_test)
