set(ENABLE_CTEST        OFF CACHE BOOL "Enable glslang tests" FORCE)
set(SHADERC_SKIP_TESTS  ON  CACHE BOOL "Skip shaderc tests" FORCE)
set(SPIRV_SKIP_TESTS    ON  CACHE BOOL "Skip SPIRV tests" FORCE)

add_subdirectory(Vulkan-Headers)
add_subdirectory(Vulkan-Loader)
add_subdirectory(glslang)
add_subdirectory(SPIRV-Headers)
add_subdirectory(SPIRV-Tools)
add_subdirectory(Vulkan-ValidationLayers)
add_subdirectory(shaderc)

target_compile_options(shaderc_util PRIVATE $<$<CONFIG:USAN>:-fno-sanitize=vptr>)
