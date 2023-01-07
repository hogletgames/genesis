list(APPEND WINDOW_PLATFORM_SOURCES
    input.cpp
    window.cpp)

list(APPEND WINDOW_PLATFORM_HEADERS
    input.h
    window.h)

ge_add_module(window-platform
    SOURCES ${WINDOW_PLATFORM_SOURCES} ${WINDOW_PLATFORM_HEADERS}
    INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC_DEPS
        genesis::core
        genesis::graphics
        genesis::math
        genesis::window-events
    PRIVATE_DEPS
        SDL2::SDL2-static)

