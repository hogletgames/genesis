ge_add_gtest(genesis_dll_test
    SOURCES
        shared_library_test.cpp
    LIBS
        genesis::dll
        genesis::filesystem
    DEPS ge_shared_library_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

add_subdirectory(shared_library_test)
