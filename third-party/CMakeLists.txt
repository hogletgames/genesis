# SDL2
set(SDL_SHARED OFF CACHE BOOL "Build a shared version of the library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build a static version of the library" FORCE)
add_subdirectory(SDL2)

# fmt
add_subdirectory(fmt)

# spdlog
set(SPDLOG_FMT_EXTERNAL CACHE BOOL "Use external fmt library instead of bundled" ON)
add_subdirectory(spdlog)

# glm
add_subdirectory(glm)

# Vulkan
add_subdirectory(Vulkan)

# stb
add_library(stb-image INTERFACE)
target_include_directories(stb-image INTERFACE stb)
target_compile_definitions(stb-image INTERFACE -DSTB_IMAGE_IMPLEMENTATION)
add_library(stb::image ALIAS stb-image)

# tinyobjloader
add_subdirectory(tinyobjloader)

# magic_enum
add_subdirectory(magic_enum)

# googletest
add_subdirectory(googletest)
target_compile_options(gtest PRIVATE $<${IS_GNU_COMPILER}:-Wno-error=maybe-uninitialized>)
