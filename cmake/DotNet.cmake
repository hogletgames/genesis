find_package(ge-dotnet REQUIRED)

function(ge_dotnet_build)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ge_dotnet_example.dll
        COMMAND ${DOTNET_EXE} build
        ARGS ${CMAKE_CURRENT_SOURCE_DIR}/ge_dotnet_example.csproj
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ge_dotnet_example.csproj
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Building .NET example"
        VERBATIM
        )
endfunction()
