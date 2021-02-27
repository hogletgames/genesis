# SDL2
set(SDL_SHARED OFF CACHE BOOL "Build a shared version of the library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build a static version of the library" FORCE)
add_subdirectory(SDL2)

# spdlog
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
