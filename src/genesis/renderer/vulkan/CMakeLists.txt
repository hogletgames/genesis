# Vulkan Renderer sources
list(APPEND GE_RENDERER_VULKAN_SRC
    instance.cpp
    render_context.cpp)

list(APPEND GE_RENDERER_VULKAN_HEADERS
    instance.h
    render_context.h
    utils.h
    vulkan_exception.h)

# Append SDL2 sources
list(APPEND GE_RENDERER_VULKAN_SRC sdl_platform_window.cpp)
list(APPEND GE_RENDERER_VULKAN_HEADERS sdl_platform_window.h)

# Vulkan Renderer library
add_library(genesis-renderer-vulkan ${GE_RENDERER_VULKAN_SRC} ${GE_RENDERER_VULKAN_HEADERS})
target_link_libraries(genesis-renderer-vulkan PUBLIC genesis::core)
target_link_libraries(genesis-renderer-vulkan PRIVATE vulkan)
target_include_directories(genesis-renderer-vulkan SYSTEM PUBLIC ${GE_THIRD_PARTY_DIR}/Vulkan/Vulkan-Headers/include)

if(NOT CMAKE_EXPORT_COMPILE_COMMANDS AND NOT GE_DISABLE_DEBUG)
    add_dependencies(genesis-renderer-vulkan VkLayer_khronos_validation-json)
endif(${GE_THIRD_PARTY_DIR}/SDL2/include)

# Add SDL2 library/include directory
target_link_libraries(genesis-renderer-vulkan PRIVATE SDL2-static)
target_include_directories(genesis-renderer-vulkan SYSTEM PRIVATE ${GE_THIRD_PARTY_DIR}/SDL2/include)

ge_default_target_config(genesis-renderer-vulkan)
add_library(genesis::renderer::vulkan ALIAS genesis-renderer-vulkan)
