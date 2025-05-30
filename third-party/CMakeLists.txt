include(ExternalProject)
include(FetchContent)

# Get number of logical cores
cmake_host_system_information(RESULT GE_NPROC QUERY NUMBER_OF_LOGICAL_CORES)

# CMake options
set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
set(FETCHCONTENT_QUIET OFF)

# Third-party libs options
set(SDL_STATIC ON)
set(SDL_TEST_ENABLED_BY_DEFAULT OFF)
set(SPDLOG_FMT_EXTERNAL ON)
set(YAML_CPP_BUILD_TOOLS OFF)
set(YAML_BUILD_SHARED_LIBS OFF)
set(YAML_CPP_INSTALL OFF)
set(YAML_CPP_FORMAT_SOURCE OFF)

# Box2D
set(BOX2D_BUILD_UNIT_TESTS OFF)
set(BOX2D_BUILD_TESTBED OFF)
set(BOX2D_BUILD_DOCS OFF)
set(BOX2D_USER_SETTINGS OFF)

# Variables
set(DEPS_DIR ${FETCHCONTENT_BASE_DIR})

# fmt
FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/hogletgames/fmt.git
    GIT_TAG 11.2.0)
FetchContent_MakeAvailable(fmt)

# glm
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/hogletgames/glm.git
    GIT_TAG 1.0.1)
FetchContent_MakeAvailable(glm)

# magic_enum
FetchContent_Declare(magic_enum
    GIT_REPOSITORY https://github.com/hogletgames/magic_enum.git
    GIT_TAG v0.9.7)
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
FetchContent_MakeAvailable(SDL2)

# spdlog
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/hogletgames/spdlog.git
    GIT_TAG v1.14.1)
FetchContent_MakeAvailable(spdlog)

# imgui
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/hogletgames/imgui.git
    GIT_TAG v1.90.4-docking
    PATCH_COMMAND ${GE_APPLY_PATCH} ${DEPS_DIR}/imgui-src ${GE_PATCHES_DIR}/imgui)
FetchContent_GetProperties(imgui)
if (NOT imgui_POPULATED)
    FetchContent_Populate(imgui)

    add_Library(ge-imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp)
    target_include_directories(ge-imgui PUBLIC ${imgui_SOURCE_DIR})

    add_library(ge-imgui-vulkan STATIC
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp)
    target_link_libraries(ge-imgui-vulkan PUBLIC
        ge-imgui
        SDL2-static
        Vulkan::Vulkan
        )
    target_include_directories(ge-imgui-vulkan PUBLIC ${imgui_SOURCE_DIR}/backends)

    add_executable(imgui_sdl_vulkan_example ${imgui_SOURCE_DIR}/examples/example_sdl2_vulkan/main.cpp)
    target_link_libraries(imgui_sdl_vulkan_example PRIVATE ge-imgui-vulkan)
    target_compile_definitions(imgui_sdl_vulkan_example PRIVATE _DEBUG)
endif ()

# ImGuizmo
FetchContent_Declare(ImGuizmo
    GIT_REPOSITORY https://github.com/hogletgames/ImGuizmo.git
    GIT_TAG ba662b119d64f9ab700bb2cd7b2781f9044f5565)
FetchContent_GetProperties(ImGuizmo)
if (NOT imguizmo_POPULATED)
    FetchContent_Populate(ImGuizmo)

    add_Library(ImGuizmo STATIC
        ${imguizmo_SOURCE_DIR}/GraphEditor.cpp
        ${imguizmo_SOURCE_DIR}/ImCurveEdit.cpp
        ${imguizmo_SOURCE_DIR}/ImGradient.cpp
        ${imguizmo_SOURCE_DIR}/ImGuizmo.cpp
        ${imguizmo_SOURCE_DIR}/ImSequencer.cpp
        )
    target_link_libraries(ImGuizmo PUBLIC ge-imgui)
    target_include_directories(ImGuizmo PUBLIC ${imguizmo_SOURCE_DIR})
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

# docopt.cpp
FetchContent_Declare(docopt.cpp
    GIT_REPOSITORY https://github.com/hogletgames/docopt.cpp.git
    GIT_TAG v0.6.3)
FetchContent_MakeAvailable(docopt.cpp)

# yaml-cpp
FetchContent_Declare(yaml-cpp
    GIT_REPOSITORY https://github.com/hogletgames/yaml-cpp.git
    GIT_TAG yaml-cpp-0.7.0)
FetchContent_MakeAvailable(yaml-cpp)

# entt
FetchContent_Declare(entt
    GIT_REPOSITORY https://github.com/hogletgames/entt.git
    GIT_TAG v3.12.2)
FetchContent_MakeAvailable(entt)

# nativefiledialog
FetchContent_Declare(nativefiledialog
    GIT_REPOSITORY https://github.com/hogletgames/nativefiledialog.git
    GIT_TAG release_116)
FetchContent_GetProperties(POPULATED nativefiledialog)
if (NOT nativefiledialog_POPULATED)
    FetchContent_Populate(nativefiledialog)

    add_library(nativefiledialog STATIC
        ${nativefiledialog_SOURCE_DIR}/src/nfd_common.c
        $<$<PLATFORM_ID:Darwin>:${nativefiledialog_SOURCE_DIR}/src/nfd_cocoa.m>
        $<$<PLATFORM_ID:Linux>:${nativefiledialog_SOURCE_DIR}/src/nfd_gtk.c>
        $<$<PLATFORM_ID:Windows>:${nativefiledialog_SOURCE_DIR}/src/nfd_win.cpp>
        )
    target_include_directories(nativefiledialog
        PRIVATE ${nativefiledialog_SOURCE_DIR}/src
        PUBLIC ${nativefiledialog_SOURCE_DIR}/src/include
        )
    if (UNIX AND NOT APPLE)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)
        target_link_libraries(nativefiledialog PRIVATE PkgConfig::GTK3)
    endif ()
    target_compile_options(nativefiledialog PRIVATE
        -Wno-deprecated-declarations
        )
endif ()

# nlohmann_json
FetchContent_Declare(nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(nlohmann_json)

# box2d
FetchContent_Declare(box2d
    GIT_REPOSITORY https://github.com/hogletgames/box2d.git
    GIT_TAG v3.0.0)
FetchContent_MakeAvailable(box2d)

# Project dependencies
if (GE_BUILD_TESTS)
    # googletest
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/hogletgames/googletest.git
        GIT_TAG release-1.12.1)
    FetchContent_MakeAvailable(googletest)
endif ()
