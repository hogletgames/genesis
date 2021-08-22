set(GE_RENDERER_INCLUDE_DIR ${GE_INCLUDE_DIR}/renderer)

list(APPEND GE_RENDERER_SRC
    gpu_command_queue.cpp
    index_buffer.cpp
    render_context.cpp
    renderer.cpp
    shader_input_layout.cpp
    vertex_buffer.cpp)

list(APPEND GE_RENDERER_HEADERS
    ${GE_RENDERER_INCLUDE_DIR}/gpu_command_queue.h
    ${GE_RENDERER_INCLUDE_DIR}/index_buffer.h
    ${GE_RENDERER_INCLUDE_DIR}/render_context.h
    ${GE_RENDERER_INCLUDE_DIR}/renderer.h
    ${GE_RENDERER_INCLUDE_DIR}/renderer_factory.h
    ${GE_RENDERER_INCLUDE_DIR}/shader_input_layout.h
    ${GE_RENDERER_INCLUDE_DIR}/vertex_buffer.h)

add_library(genesis-renderer STATIC ${GE_RENDERER_SRC} ${GE_RENDERER_HEADERS})
target_link_libraries(genesis-renderer PUBLIC
    genesis::core
    genesis::renderer::vulkan)
target_include_directories(genesis-renderer PRIVATE ${GE_RENDERER_INCLUDE_DIR})
ge_default_target_config(genesis-renderer)
add_library(genesis::renderer ALIAS genesis-renderer)

add_subdirectory(vulkan)
