include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
set(FETCHCONTENT_QUIET OFF)

set(DEPS_DIR ${FETCHCONTENT_BASE_DIR})

# fmt
FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/hogletgames/fmt.git
    GIT_TAG 9.1.0)
FetchContent_MakeAvailable(fmt)

# glm
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/hogletgames/glm.git
    GIT_TAG 0.9.9.8)
FetchContent_MakeAvailable(glm)

# magic_enum
FetchContent_Declare(magic_enum
    GIT_REPOSITORY https://github.com/hogletgames/magic_enum.git
    GIT_TAG v0.7.2)
FetchContent_MakeAvailable(magic_enum)

# tinyobjloader
FetchContent_Declare(tinyobjloader
    GIT_REPOSITORY https://github.com/hogletgames/tinyobjloader.git
    GIT_TAG v2.0.0rc10)
FetchContent_MakeAvailable(tinyobjloader)

# SDL2
FetchContent_Declare(SDL2
    GIT_REPOSITORY https://github.com/hogletgames/SDL.git
    GIT_TAG release-2.26.2)
set(SDL_SHARED OFF CACHE BOOL "Build a shared version of the library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build a static version of the library" FORCE)
FetchContent_MakeAvailable(SDL2)

# spdlog
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/hogletgames/spdlog.git
    GIT_TAG v1.11.0)
set(SPDLOG_FMT_EXTERNAL ON)
FetchContent_MakeAvailable(spdlog)

# imgui
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/hogletgames/imgui.git
    GIT_TAG dc8c3618e8f8e2dada23daa1aa237626af341fd8
    PATCH_COMMAND ${GE_APPLY_PATCH} ${DEPS_DIR}/imgui-src ${GE_PATCHES_DIR}/imgui)
FetchContent_GetProperties(imgui)
if (NOT imgui_POPULATED)
    FetchContent_Populate(imgui)

    add_Library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp)
    target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})

    add_library(imgui-vulkan STATIC
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp)
    target_link_libraries(imgui-vulkan PUBLIC
        imgui
        SDL2-static
        Vulkan::Headers
        Vulkan::Vulkan
        )
    target_include_directories(imgui-vulkan PUBLIC ${imgui_SOURCE_DIR}/backends)

    add_executable(imgui_sdl_vulkan_example ${imgui_SOURCE_DIR}/examples/example_sdl_vulkan/main.cpp)
    target_link_libraries(imgui_sdl_vulkan_example PRIVATE imgui-vulkan)
    target_compile_definitions(imgui_sdl_vulkan_example PRIVATE _DEBUG)
endif ()

# stb
FetchContent_Declare(stb
    GIT_REPOSITORY https://github.com/hogletgames/stb.git
    GIT_TAG 8b5f1f37b5b75829fc72d38e7b5d4bcbf8a26d55)
FetchContent_GetProperties(stb)
if (NOT stb_POPULATED)
    FetchContent_Populate(stb)
    add_library(stb-image INTERFACE)
    target_include_directories(stb-image INTERFACE ${stb_SOURCE_DIR})
    target_compile_options(stb-image INTERFACE
        $<${IS_UNIX_COMPILER}:-Wno-sign-compare;-Wno-unused-but-set-variable>)
    target_compile_definitions(stb-image INTERFACE -DSTB_IMAGE_IMPLEMENTATION)
endif ()

# Vulkan SDK config variables
set(UPDATE_DEPS ON)

# Vulkan-Loader
FetchContent_Declare(Vulkan-Loader
    GIT_REPOSITORY https://github.com/hogletgames/Vulkan-Loader.git
    GIT_TAG sdk-1.3.236.0)
FetchContent_MakeAvailable(Vulkan-Loader)

# glslang
FetchContent_Declare(glslang
    GIT_REPOSITORY https://github.com/hogletgames/glslang.git
    GIT_TAG sdk-1.3.236.0
    PATCH_COMMAND ${GE_APPLY_PATCH} ${DEPS_DIR}/glslang-src ${GE_PATCHES_DIR}/glslang)
set(ENABLE_CTEST OFF)
FetchContent_MakeAvailable(glslang)

# SPIRV-Headers
FetchContent_Declare(SPIRV-Headers
    GIT_REPOSITORY https://github.com/hogletgames/SPIRV-Headers.git
    GIT_TAG sdk-1.3.236.0)
FetchContent_MakeAvailable(SPIRV-Headers)

# SPIRV-Tools
FetchContent_Declare(SPIRV-Tools
    GIT_REPOSITORY https://github.com/hogletgames/SPIRV-Tools.git
    GIT_TAG sdk-1.3.236.0)
FetchContent_MakeAvailable(SPIRV-Tools)
set(DISABLE_STRINGOP_TRUNCATION $<$<AND:${IS_GNU_COMPILER},${IS_800_VERSION}>:-Wno-error=stringop-truncation>)
target_compile_options(SPIRV-Tools-static PRIVATE ${DISABLE_STRINGOP_TRUNCATION})
target_compile_options(SPIRV-Tools-shared PRIVATE ${DISABLE_STRINGOP_TRUNCATION})

# SPIRV-Cross
FetchContent_Declare(SPIRV-Cross
    GIT_REPOSITORY https://github.com/hogletgames/SPIRV-Cross.git
    GIT_TAG sdk-1.3.236.0)
FetchContent_MakeAvailable(SPIRV-Cross)

# Vulkan-ValidationLayers
FetchContent_Declare(Vulkan-ValidationLayers
    GIT_REPOSITORY https://github.com/hogletgames/Vulkan-ValidationLayers.git
    GIT_TAG sdk-1.3.236.0)
FetchContent_MakeAvailable(Vulkan-ValidationLayers)

# shaderc
FetchContent_Declare(shaderc
    GIT_REPOSITORY https://github.com/hogletgames/shaderc.git
    GIT_TAG v2022.4)
set(SHADERC_SKIP_TESTS ON)
FetchContent_MakeAvailable(shaderc)
target_compile_options(shaderc_util PRIVATE $<$<CONFIG:USAN>:-fno-sanitize=vptr>)

# Vulkan-Headers
FetchContent_Declare(Vulkan-Headers
    GIT_REPOSITORY https://github.com/hogletgames/Vulkan-Headers.git
    GIT_TAG sdk-1.3.236.0)
FetchContent_MakeAvailable(Vulkan-Headers)

# Apple dependencies
if (APPLE)
    # MoltenVK
    FetchContent_Declare(MoltenVK
        GIT_REPOSITORY https://github.com/hogletgames/MoltenVK.git
        GIT_TAG v1.2.1)
    if (NOT MoltenVK_POPULATED)
        FetchContent_Populate(MoltenVK)
        include(ExternalProject)
        ExternalProject_Add(libMoltenVK
            SOURCE_DIR ${moltenvk_SOURCE_DIR}
            BUILD_IN_SOURCE ON
            CONFIGURE_COMMAND ./fetchDependencies --macos
            BUILD_COMMAND make macos
            INSTALL_COMMAND "")
        add_dependencies(vulkan libMoltenVK)
    endif()
endif ()

# Project dependencies
if (GE_BUILD_TESTS)
    # googletest
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/hogletgames/googletest.git
        GIT_TAG release-1.12.0)
    FetchContent_MakeAvailable(googletest)
endif ()

if (NOT GE_DISABLE_DEBUG)
    add_dependencies(vulkan VkLayer_khronos_validation)
endif ()
