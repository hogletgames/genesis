list(APPEND SANDBOX_SRC
    render_layer.cpp
    sandbox.cpp)

list(APPEND SANDBOX_HEADERS
    render_layer.h)

add_executable(sandbox ${SANDBOX_SRC} ${SANDBOX_HEADERS})
target_link_libraries(sandbox PRIVATE genesis::genesis)
ge_default_target_config(sandbox)
