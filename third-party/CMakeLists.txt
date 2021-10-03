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

# imgui
add_Library(imgui STATIC
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp)
target_include_directories(imgui PUBLIC imgui)

# imgui Vulkan SDL2
add_library(imgui-vulkan STATIC
    imgui/backends/imgui_impl_sdl.cpp
    imgui/backends/imgui_impl_vulkan.cpp)
target_link_libraries(imgui-vulkan PUBLIC
    imgui
    SDL2-static
    vulkan)
target_include_directories(imgui-vulkan PUBLIC imgui/backends)

# ImGui SDL Vulcan example
# NOTE: imgui patch below must be commented to be able to run this example
# add_executable(imgui_sdl_vulkan_example imgui/examples/example_sdl_vulkan/main.cpp)
# target_link_libraries(imgui_sdl_vulkan_example PRIVATE imgui-vulkan)
# target_compile_definitions(imgui_sdl_vulkan_example PRIVATE _DEBUG)

# patches
ge_apply_patch(imgui)
