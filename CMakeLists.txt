cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(genesis VERSION 0.0.0 LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "Build type is empty, configuring as 'Release'")
    set(CMAKE_BUILD_TYPE "Release")
endif ()

# CMake options
option(GE_STATIC "Build static library" OFF)
option(GE_DISABLE_ASSERTS "Disable asserts" OFF)
option(GE_BUILD_EXAMPLES "Build examples" OFF)
option(GE_BUILD_TESTS "Build tests" OFF)

# CMake modules
include(cmake/AddModule.cmake)
include(cmake/ApplyPatch.cmake)
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
message(STATUS "Build examples: ${GE_BUILD_EXAMPLES}")
message(STATUS "Build tests: ${GE_BUILD_TESTS}")

# Build options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Subdirectories
add_subdirectory(src)
add_subdirectory(third-party)

if (GE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

if (GE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()
