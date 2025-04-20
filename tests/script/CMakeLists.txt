ge_add_gtest(genesis_script_test
    SOURCES
        host_framework_test.cpp
        main.cpp
    LIBS
        genesis::filesystem
        genesis::script
    DO_NOT_LINK_MAIN
    )

file(COPY        data
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)
