include(${PROJECT_SOURCE_DIR}/cmake/DotNetHelpers.cmake)

ge_add_executable(dotnet_example
    SOURCES dotnet_example.cpp
    PRIVATE_DEPS
        genesis::core
        hostfxr
        nethost
    )

ge_dotnet_build(dotnet-script/DotNetExample.csproj TARGET DotNetExample)
add_dependencies(dotnet_example DotNetExample)
target_compile_definitions(dotnet_example PRIVATE
    DOTNET_EXAMPLE_DLL_DIR="${CMAKE_CURRENT_BINARY_DIR}/DotNetExample"
    )
