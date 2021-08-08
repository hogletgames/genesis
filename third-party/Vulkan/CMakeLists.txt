# Build options
set(ENABLE_CTEST        OFF CACHE BOOL "Enable glslang tests" FORCE)
set(SHADERC_SKIP_TESTS  ON  CACHE BOOL "Skip shaderc tests" FORCE)
set(SPIRV_SKIP_TESTS    ON  CACHE BOOL "Skip SPIRV tests" FORCE)

# Submodules
add_subdirectory(Vulkan-Headers)
add_subdirectory(Vulkan-Loader)
add_subdirectory(glslang)
add_subdirectory(SPIRV-Headers)
add_subdirectory(SPIRV-Tools)
add_subdirectory(SPIRV-Cross)
add_subdirectory(Vulkan-ValidationLayers)
add_subdirectory(shaderc)

# Compile options
list(APPEND SPIRV_TOOLS_TARGETS SPIRV-Tools-static SPIRV-Tools-shared)

foreach(SPIRV_TOOLS_TARGET IN LISTS SPIRV_TOOLS_TARGETS)
    target_compile_options(${SPIRV_TOOLS_TARGET} PRIVATE
        $<$<VERSION_GREATER_EQUAL:$<CXX_COMPILER_VERSION>,8.0.0>:-Wno-error=stringop-truncation>)
endforeach()

target_compile_options(shaderc_util PRIVATE $<$<CONFIG:USAN>:-fno-sanitize=vptr>)
