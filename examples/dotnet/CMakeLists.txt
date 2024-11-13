include(${PROJECT_SOURCE_DIR}/cmake/DotNetHelpers.cmake)

ge_add_executable(dotnet_example
    SOURCES dotnet_example.cpp
    PRIVATE_DEPS
        dl
        ge-dotnet-example-pinvoke
        ge-dotnet-example-bindings
        genesis::core
        nethost
    )

ge_dotnet_build(DotNetExample
    dotnet-script/DotNetExample.csproj
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dotnet-pinvoke)
add_dependencies(dotnet_example DotNetExample)
target_compile_definitions(dotnet_example PRIVATE
    DOTNET_EXAMPLE_DLL_DIR="${CMAKE_CURRENT_BINARY_DIR}/dotnet-pinvoke")

add_subdirectory(dotnet-pinvoke)
