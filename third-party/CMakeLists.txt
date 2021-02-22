unset(CMAKE_CXX_CLANG_TIDY)

# SDL2
set(SDL_SHARED OFF CACHE BOOL "Build a shared version of the library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build a static version of the library" FORCE)
add_subdirectory(SDL2)
target_compile_options(SDL2-static PRIVATE
    -Wno-unused-parameter
    -Wno-sign-compare
    -Wno-pedantic
    -Wno-cast-function-type
    -Wno-format
    -Wno-implicit-fallthrough
    -Wno-shadow
)

# spdlog
add_subdirectory(spdlog)

# glm
add_subdirectory(glm)
