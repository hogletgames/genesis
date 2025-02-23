include(${PROJECT_SOURCE_DIR}/cmake/GeMonoHelpers.cmake)

ge_mono_create_assembly(GeFirstAssembly.dll SOURCES data/FirstAssembly.cs)
ge_mono_create_assembly(GeMethodTests.dll SOURCES data/MethodTests.cs)
ge_mono_create_assembly(GeSecondAssemblyTest.dll SOURCES data/SecondAssembly.cs)

ge_add_gtest(genesis_script_test
    SOURCES
        assembly_test.cpp
        main.cpp
        domain_test.cpp
        method_test.cpp
        tested_value.h
    LIBS
        genesis::filesystem
        genesis::script
    DEPS
        GeFirstAssembly.dll
        GeMethodTests.dll
        GeSecondAssemblyTest.dll
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DO_NOT_LINK_MAIN
    )
