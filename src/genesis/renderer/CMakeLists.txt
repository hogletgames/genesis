set(GE_RENDERER_INCLUDE_DIR ${GE_INCLUDE_DIR}/renderer)

list(APPEND GE_RENDERER_SRC
    framebuffer.cpp
    index_buffer.cpp
    render_context.cpp
    renderer.cpp
    renderer_factory.cpp
    vertex_array.cpp
    vertex_buffer.cpp)

list(APPEND GE_RENDERER_HEADERS
    ${GE_RENDERER_INCLUDE_DIR}/framebuffer.h
    ${GE_RENDERER_INCLUDE_DIR}/index_buffer.h
    ${GE_RENDERER_INCLUDE_DIR}/render_context.h
    ${GE_RENDERER_INCLUDE_DIR}/renderer.h
    ${GE_RENDERER_INCLUDE_DIR}/renderer_factory.h
    ${GE_RENDERER_INCLUDE_DIR}/vertex_array.h
    ${GE_RENDERER_INCLUDE_DIR}/vertex_buffer.h)

add_library(genesis-renderer STATIC ${GE_RENDERER_SRC} ${GE_RENDERER_HEADERS})
target_link_libraries(genesis-renderer PUBLIC genesis::core)
target_link_libraries(genesis-renderer PRIVATE genesis::renderer::vulkan)
target_include_directories(genesis-renderer PRIVATE ${GE_RENDERER_INCLUDE_DIR})
ge_default_target_config(genesis-renderer)
add_library(genesis::renderer ALIAS genesis-renderer)

add_subdirectory(vulkan)
