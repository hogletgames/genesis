cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(genesis LANGUAGES CXX)

# CMake options
option(GE_STATIC "Build static library" OFF)

set(GE_THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third-party)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Build type is empty, configuring as 'Release'")
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Build options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Werror -Wextra -Wall -Wpedantic -Wshadow)

include_directories(include)

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

# Subdirectories
add_subdirectory(third-party)
