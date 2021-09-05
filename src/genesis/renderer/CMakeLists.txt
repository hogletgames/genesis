set(GE_RENDERER_INCLUDE_DIR ${GE_INCLUDE_DIR}/renderer)

list(APPEND GE_RENDERER_SRC
    gpu_command_queue.cpp
    render_context.cpp
    renderer.cpp)

list(APPEND GE_RENDERER_HEADERS
    ${GE_RENDERER_INCLUDE_DIR}/gpu_command_queue.h
    ${GE_RENDERER_INCLUDE_DIR}/render_context.h
    ${GE_RENDERER_INCLUDE_DIR}/renderer.h
    ${GE_RENDERER_INCLUDE_DIR}/renderer_factory.h)

add_library(genesis-renderer STATIC ${GE_RENDERER_SRC} ${GE_RENDERER_HEADERS})
target_link_libraries(genesis-renderer PUBLIC
    genesis::core
    genesis::renderer::vulkan)
target_include_directories(genesis-renderer PRIVATE ${GE_RENDERER_INCLUDE_DIR})
ge_default_target_config(genesis-renderer)
add_library(genesis::renderer ALIAS genesis-renderer)

add_subdirectory(vulkan)
