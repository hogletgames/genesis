set(GE_RENDERER_INCLUDE_DIR ${GE_INCLUDE_DIR}/graphics)

list(APPEND GE_RENDERER_SRC
    gpu_command_queue.cpp
    graphics.cpp
    graphics_context.cpp
    index_buffer.cpp
    render_command.cpp
    shader.cpp
    shader_input_layout.cpp
    shader_precompiler.cpp
    shader_program.cpp
    shader_reflection.cpp
    vertex_buffer.cpp)

list(APPEND GE_RENDERER_HEADERS
    ${GE_RENDERER_INCLUDE_DIR}/gpu_command_queue.h
    ${GE_RENDERER_INCLUDE_DIR}/graphics.h
    ${GE_RENDERER_INCLUDE_DIR}/graphics_context.h
    ${GE_RENDERER_INCLUDE_DIR}/graphics_factory.h
    ${GE_RENDERER_INCLUDE_DIR}/index_buffer.h
    ${GE_RENDERER_INCLUDE_DIR}/render_command.h
    ${GE_RENDERER_INCLUDE_DIR}/shader.h
    ${GE_RENDERER_INCLUDE_DIR}/shader_input_layout.h
    ${GE_RENDERER_INCLUDE_DIR}/shader_precompiler.h
    ${GE_RENDERER_INCLUDE_DIR}/shader_program.h
    ${GE_RENDERER_INCLUDE_DIR}/shader_reflection.h
    ${GE_RENDERER_INCLUDE_DIR}/texture.h
    ${GE_RENDERER_INCLUDE_DIR}/vertex_buffer.h)

add_library(genesis-graphics STATIC ${GE_RENDERER_SRC} ${GE_RENDERER_HEADERS})
target_link_libraries(genesis-graphics
    PUBLIC
        genesis::core
    PRIVATE
        genesis::graphics::vulkan
        shaderc
        spirv-cross-cpp)
target_include_directories(genesis-graphics PRIVATE ${GE_RENDERER_INCLUDE_DIR})
target_include_directories(genesis-graphics SYSTEM PRIVATE
    ${GE_THIRD_PARTY_DIR}/Vulkan/shaderc/libshaderc/include
    ${GE_THIRD_PARTY_DIR}/Vulkan/SPIRV-Cross)
ge_default_target_config(genesis-graphics)
add_library(genesis::graphics ALIAS genesis-graphics)

add_subdirectory(vulkan)
