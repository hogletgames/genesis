include(${PROJECT_SOURCE_DIR}/cmake/DotNetHelpers.cmake)
ge_dotnet_build(TestAssembly/TestAssembly.csproj)

ge_add_gtest(genesis_script_test
    SOURCES
        assembly_manager_test.cpp
        host_framework_test.cpp
        main.cpp
    LIBS
        genesis::filesystem
        genesis::script
    DEPS
        GenesisScripting
        TestAssembly
    DEFINITIONS
        GE_SCRIPTING_DIR="${GenesisScripting_BINARY_DIR}"
        TEST_ASSEMBLY_DIR="${TestAssembly_BINARY_DIR}"
    DO_NOT_LINK_MAIN
    )

file(COPY        data
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)
