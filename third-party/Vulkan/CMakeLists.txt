add_subdirectory(Vulkan-Headers)
add_subdirectory(Vulkan-Loader)
add_subdirectory(glslang)
add_subdirectory(SPIRV-Headers)
add_subdirectory(SPIRV-Tools)
add_subdirectory(Vulkan-ValidationLayers)

# shaderc
set(SHADERC_SKIP_TESTS ON)
add_subdirectory(shaderc)
target_compile_options(shaderc_util PRIVATE $<$<CONFIG:USAN>:-fno-sanitize=vptr>)
