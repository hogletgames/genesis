cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(genesis VERSION 0.0.0 LANGUAGES CXX)

# CMake options
option(GE_STATIC "Build static library" OFF)
option(GE_DISABLE_ASSERTS "Disable asserts" OFF)
option(GE_BUILD_EXAMPLES "Build examples" OFF)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(GE_DISABLE_DEBUG True)
endif()

include(cmake/SanitizeHelpers.cmake)
include(cmake/Utils.cmake)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Build type is empty, configuring as 'Release'")
    set(CMAKE_BUILD_TYPE "Release")
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "${CMAKE_PROJECT_NAME} version ${CMAKE_PROJECT_VERSION}")

# Build options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform specific macros
if(UNIX)
    message(STATUS "Platform: Unix")
    add_definitions(-DGE_PLATFORM_UNIX)
elseif(WIN32)
    message(STATUS "Platform: Windows")
    add_definitions(-DGE_PLATFORM_WINDOWS)
endif()

# Project configuration
if(GE_STATIC)
    message(STATUS "Building STATIC library")
    set(GE_LIB_TYPE STATIC)
else()
    message(STATUS "Building SHARED library")
    set(GE_LIB_TYPE SHARED)
endif()

if(GE_DISABLE_ASSERTS)
    message(STATUS "Asserts are disabled")
    add_definitions(-DGE_DISABLE_ASSERTS)
else()
    message(STATUS "Asserts are enabled")
endif()

if(GE_DISABLE_DEBUG)
    message(STATUS "Disable debug output")
    add_definitions(-DGE_DISABLE_DEBUG)
else()
    message(STATUS "Keep debug output")
endif()

# Subdirectories
add_subdirectory(src)

if(GE_BUILD_EXAMPLES)
    message(STATUS "Build '${CMAKE_PROJECT_NAME}' examples")
    add_subdirectory(examples)
endif()

if(NOT CMAKE_EXPORT_COMPILE_COMMANDS)
    add_subdirectory(third-party)
endif()
