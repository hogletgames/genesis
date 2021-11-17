# Vulkan Graphics sources
list(APPEND GE_RENDERER_VULKAN_SRC
    buffers/buffer_base.cpp
    buffers/index_buffer.cpp
    buffers/staging_buffer.cpp
    buffers/vertex_buffer.cpp
    device.cpp
    graphics_context.cpp
    image.cpp
    input_stage_descriptions.cpp
    instance.cpp
    pipeline.cpp
    renderer_factory.cpp
    sdl_gui_context.cpp
    shader.cpp
    shader_program.cpp
    single_command.cpp
    swap_chain.cpp)

list(APPEND GE_RENDERER_VULKAN_HEADERS
    buffers/buffer_base.h
    buffers/index_buffer.h
    buffers/staging_buffer.h
    buffers/vertex_buffer.h
    command_buffer.h
    device.h
    graphics_context.h
    image.h
    input_stage_descriptions.h
    instance.h
    pipeline.h
    renderer_factory.h
    shader.h
    shader_program.h
    single_command.h
    sdl_gui_context.h
    swap_chain.h
    utils.h
    vulkan_exception.h)

# Append SDL2 sources
list(APPEND GE_RENDERER_VULKAN_SRC sdl_platform_window.cpp)
list(APPEND GE_RENDERER_VULKAN_HEADERS sdl_platform_window.h)

# Vulkan Graphics library
add_library(genesis-graphics-vulkan ${GE_RENDERER_VULKAN_SRC} ${GE_RENDERER_VULKAN_HEADERS})
target_link_libraries(genesis-graphics-vulkan
    PUBLIC
        genesis::core
        genesis::graphics
        genesis::math
        vulkan
    PRIVATE
        imgui-vulkan)
target_include_directories(genesis-graphics-vulkan PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(genesis-graphics-vulkan SYSTEM
    PUBLIC
        ${GE_THIRD_PARTY_DIR}/Vulkan/Vulkan-Headers/include
    PRIVAT
        ${GE_THIRD_PARTY_DIR}/imgui
        ${GE_THIRD_PARTY_DIR}/imgui/backends)

if (NOT CMAKE_EXPORT_COMPILE_COMMANDS AND NOT GE_DISABLE_DEBUG)
    add_dependencies(genesis-graphics-vulkan VkLayer_khronos_validation-json)
endif ()

# Add SDL2 library/include directory
target_link_libraries(genesis-graphics-vulkan PRIVATE SDL2-static)
target_include_directories(genesis-graphics-vulkan SYSTEM PRIVATE ${GE_THIRD_PARTY_DIR}/SDL2/include)

ge_default_target_config(genesis-graphics-vulkan)
add_library(genesis::graphics::vulkan ALIAS genesis-graphics-vulkan)
