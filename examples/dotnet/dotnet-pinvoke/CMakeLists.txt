add_library(ge-dotnet-example-pinvoke STATIC
    object.cpp
    object.h
    )
target_include_directories(ge-dotnet-example-pinvoke PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ge-dotnet-example-pinvoke PRIVATE genesis::core)

add_library(ge-dotnet-example-bindings SHARED
    bindings.cpp
    bindings.h
    )
target_include_directories(ge-dotnet-example-bindings PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ge-dotnet-example-bindings PRIVATE
    ge-dotnet-example-pinvoke
    genesis::core
    )
