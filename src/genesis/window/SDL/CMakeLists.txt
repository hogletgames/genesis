list(APPEND GE_WINDOW_SDL_SRC
    window.cpp
)

list(APPEND GE_WINDOW_SDL_HEADERS
    window.h
)

add_library(genesis-window-sdl STATIC ${GE_WINDOW_SDL_SRC} ${GE_WINDOW_SDL_HEADERS})
target_link_libraries(genesis-window-sdl PUBLIC
    genesis::core
    genesis::math
    genesis::renderer
)
target_link_libraries(genesis-window-sdl PRIVATE SDL2-static)
target_include_directories(genesis-window-sdl SYSTEM PRIVATE ${GE_THIRD_PARTY_DIR}/SDL2/include)
ge_default_target_config(genesis-window-sdl)
add_library(genesis::window::platform ALIAS genesis-window-sdl)
