add_executable(sdl2_vulkan_example sdl2_vulkan_example.cpp)
target_link_libraries(sdl2_vulkan_example PRIVATE
    genesis::core
    glm
    SDL2-static
    shaderc_shared
    stb-image
    tinyobjloader
    vulkan
)
target_include_directories(sdl2_vulkan_example SYSTEM PRIVATE
    ${GE_THIRD_PARTY_DIR}/glm
    ${GE_THIRD_PARTY_DIR}/SDL2/include
    ${GE_THIRD_PARTY_DIR}/stb
    ${GE_THIRD_PARTY_DIR}/tinyobjloader
    ${GE_THIRD_PARTY_DIR}/Vulkan/shaderc/libshaderc/include
    ${GE_THIRD_PARTY_DIR}/Vulkan/Vulkan-Headers/include
)
if(NOT CMAKE_EXPORT_COMPILE_COMMANDS)
    add_dependencies(sdl2_vulkan_example VkLayer_khronos_validation-json)
endif()
ge_default_target_config(sdl2_vulkan_example)
