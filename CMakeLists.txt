cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
project(genesis VERSION 0.0.0 LANGUAGES C CXX)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "Build type is empty, configuring as 'Release'")
    set(CMAKE_BUILD_TYPE "Release")
endif ()

# Configure custom find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/find-modules")

# CMake options
option(GE_STATIC "Build static library" OFF)
option(GE_DISABLE_ASSERTS "Disable asserts" OFF)
option(GE_BUILD_APPS "Build applications" OFF)
option(GE_BUILD_EXAMPLES "Build examples" OFF)
option(GE_BUILD_TESTS "Build tests" OFF)

# CMake modules
include(cmake/AddModule.cmake)
include(cmake/ApplyPatch.cmake)
include(cmake/BuildType.cmake)
include(cmake/CompilerOptions.cmake)
include(cmake/SanitizeHelpers.cmake)
include(cmake/Macro.cmake)
configure_file(cmake/CTestCustom.cmake ${CMAKE_BINARY_DIR})

# Process options
ge_configure_debug_status()
ge_ternary(GE_LIB_TYPE ${GE_STATIC} STATIC SHARED)

# Print status
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Build static library: ${GE_STATIC}")
message(STATUS "Disable debug output: ${GE_DISABLE_DEBUG}")
message(STATUS "Disable asserts: ${GE_DISABLE_ASSERTS}")
message(STATUS "Build applications: ${GE_BUILD_APPS}")
message(STATUS "Build examples: ${GE_BUILD_EXAMPLES}")
message(STATUS "Build tests: ${GE_BUILD_TESTS}")

# Build options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find required packages
find_package(Boost "1.82" COMPONENTS filesystem REQUIRED)
find_package(ge-nethost REQUIRED)
find_package(ge-shaderc_combined REQUIRED)
find_package(ge-spirv-cross REQUIRED)
find_package(Vulkan REQUIRED)

# Subdirectories
add_subdirectory(src)
add_subdirectory(third-party)

if (GE_BUILD_APPS)
    add_subdirectory(apps)
endif ()

if (GE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

if (GE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()
