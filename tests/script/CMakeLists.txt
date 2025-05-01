include(${PROJECT_SOURCE_DIR}/cmake/DotNetHelpers.cmake)

ge_add_gtest(genesis_script_test
    SOURCES
        assembly_manager_test.cpp
        delegate_loader_test.cpp
        host_framework_test.cpp
        main.cpp
    LIBS
        genesis::filesystem
        genesis::script
    DEPS
        TestAssembly
    DO_NOT_LINK_MAIN
    )

ge_dotnet_build(TestAssembly/TestAssembly.csproj)

file(COPY        data
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/data)
