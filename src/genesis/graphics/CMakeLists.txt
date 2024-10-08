set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/graphics)

list(APPEND GRAPHICS_SOURCES
    framebuffer.cpp
    graphics.cpp
    index_buffer.cpp
    mesh.cpp
    primitives_renderer.cpp
    render_command.cpp
    shader.cpp
    shader_input_layout.cpp
    shader_precompiler.cpp
    shader_reflection.cpp
    staging_buffer.cpp
    texture.cpp
    texture_loader.cpp
    vertex_buffer.cpp
    uniform_buffer.cpp
    )

list(APPEND GRAPHICS_HEADERS
    ${INCLUDE_DIR}/framebuffer.h
    ${INCLUDE_DIR}/gpu_command_queue.h
    ${INCLUDE_DIR}/graphics.h
    ${INCLUDE_DIR}/graphics_context.h
    ${INCLUDE_DIR}/graphics_factory.h
    ${INCLUDE_DIR}/index_buffer.h
    ${INCLUDE_DIR}/mesh.h
    ${INCLUDE_DIR}/primitives_renderer.h
    ${INCLUDE_DIR}/pipeline.h
    ${INCLUDE_DIR}/pipeline_config.h
    ${INCLUDE_DIR}/render_command.h
    ${INCLUDE_DIR}/renderer.h
    ${INCLUDE_DIR}/shader.h
    ${INCLUDE_DIR}/shader_input_layout.h
    ${INCLUDE_DIR}/shader_precompiler.h
    ${INCLUDE_DIR}/shader_reflection.h
    ${INCLUDE_DIR}/shader_resource_descriptors.h
    ${INCLUDE_DIR}/staging_buffer.h
    ${INCLUDE_DIR}/texture.h
    ${INCLUDE_DIR}/texture_loader.h
    ${INCLUDE_DIR}/uniform_buffer.h
    ${INCLUDE_DIR}/vertex.h
    ${INCLUDE_DIR}/vertex_buffer.h
    )

ge_add_module(graphics
    SOURCES ${GRAPHICS_SOURCES} ${GRAPHICS_HEADERS}
    INCLUDE_DIRS ${INCLUDE_DIR}
    PUBLIC_DEPS
        genesis::core
        genesis::math
    PRIVATE_DEPS
        genesis::graphics-vulkan
        shaderc_combined
        spirv-cross-cpp
        stb-image
        tinyobjloader
    )

add_subdirectory(vulkan)
