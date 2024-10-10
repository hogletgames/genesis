include(${PROJECT_SOURCE_DIR}/cmake/DotNet.cmake)

ge_add_executable(dotnet_example
    SOURCES dotnet_example.cpp
    PRIVATE_DEPS
        hostfxr
        nethost
    )


