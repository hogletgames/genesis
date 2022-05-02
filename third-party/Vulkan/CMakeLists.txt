# Build options
set(ENABLE_CTEST        OFF CACHE BOOL "Enable glslang tests" FORCE)
set(SHADERC_SKIP_TESTS  ON  CACHE BOOL "Skip shaderc tests" FORCE)
set(SPIRV_SKIP_TESTS    ON  CACHE BOOL "Skip SPIRV tests" FORCE)

# Patches
ge_apply_patch(glslang)
ge_apply_patch(Vulkan-ValidationLayers)

# Submodules
add_subdirectory(Vulkan-Headers)
add_subdirectory(Vulkan-Loader)
add_subdirectory(glslang)
add_subdirectory(SPIRV-Headers)
add_subdirectory(SPIRV-Tools)
add_subdirectory(SPIRV-Cross)
add_subdirectory(Vulkan-ValidationLayers)
add_subdirectory(shaderc)

if (APPLE)
    add_subdirectory(MoltenVK)
endif ()

# Compile options
list(APPEND SPIRV_TOOLS_TARGETS SPIRV-Tools-static SPIRV-Tools-shared)

foreach(SPIRV_TOOLS_TARGET IN LISTS SPIRV_TOOLS_TARGETS)
    target_compile_options(${SPIRV_TOOLS_TARGET} PRIVATE
        $<$<AND:${IS_GNU_COMPILER},${IS_800_VERSION}>:-Wno-error=stringop-truncation>)
endforeach()

target_compile_options(shaderc_util PRIVATE $<$<CONFIG:USAN>:-fno-sanitize=vptr>)

# Dependencies
if (NOT GE_DISABLE_DEBUG)
    add_dependencies(vulkan VkLayer_khronos_validation-json)
endif ()
