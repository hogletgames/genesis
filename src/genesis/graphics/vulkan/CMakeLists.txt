list(APPEND GRAPHICS_VULKAN_SOURCES
    buffers/buffer_base.cpp
    buffers/index_buffer.cpp
    buffers/staging_buffer.cpp
    buffers/vertex_buffer.cpp
    renderers/framebuffer_renderer.cpp
    renderers/renderer_base.cpp
    renderers/window_renderer.cpp
    device.cpp
    graphics_context.cpp
    graphics_factory.cpp
    framebuffer.cpp
    image.cpp
    pipeline_barrier.cpp
    input_stage_descriptions.cpp
    instance.cpp
    pipeline.cpp
    sdl_gui_context.cpp
    shader.cpp
    single_command.cpp
    swap_chain.cpp
    utils.cpp)

list(APPEND GRAPHICS_VULKAN_HEADERS
    buffers/buffer_base.h
    buffers/index_buffer.h
    buffers/staging_buffer.h
    buffers/vertex_buffer.h
    renderers/framebuffer_renderer.h
    renderers/renderer_base.h
    renderers/window_renderer.h
    command_buffer.h
    device.h
    graphics_context.h
    graphics_factory.h
    framebuffer.h
    image.h
    pipeline_barrier.h
    input_stage_descriptions.h
    instance.h
    pipeline.h
    shader.h
    single_command.h
    sdl_gui_context.h
    swap_chain.h
    texture.cpp
    texture.h
    utils.h
    vulkan_exception.h)

ge_add_module(graphics-vulkan
    SOURCES ${GRAPHICS_VULKAN_SOURCES} ${GRAPHICS_VULKAN_HEADERS}
    INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC_DEPS
        genesis::core
        genesis::graphics
        genesis::math
        Vulkan::Headers
        Vulkan::Vulkan
    PRIVATE_DEPS
        imgui-vulkan
        SDL2-static
        shaderc
        spirv-cross-cpp
        stb-image)

if (NOT GE_DISABLE_DEBUG)
    add_dependencies(genesis-graphics-vulkan VkLayer_khronos_validation)
endif ()
