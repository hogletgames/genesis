ge_add_executable(sdl2_vulkan_example
    SOURCES sdl2_vulkan_example.cpp
    PRIVATE_DEPS
        genesis::core
        glm
        SDL2::SDL2-static
        shaderc_combined
        stb-image
        tinyobjloader
        Vulkan::Vulkan
    )
