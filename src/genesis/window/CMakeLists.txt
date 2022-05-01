set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/window)

list(APPEND WINDOW_SOURCES
    input.cpp
    window.cpp)

list(APPEND WINDOW_HEADERS
    ${INCLUDE_DIR}/input.h
    ${INCLUDE_DIR}/key_codes.h
    ${INCLUDE_DIR}/mouse_button_codes.h
    ${INCLUDE_DIR}/window.h)

ge_add_module(window
    SOURCES ${WINDOW_SOURCES} ${WINDOW_HEADERS}
    INCLUDE_DIR ${INCLUDE_DIR}
    PUBLIC_DEPS
        genesis::core
        genesis::math
        genesis::window-events
        genesis::window-platform)

add_subdirectory(events)
add_subdirectory(SDL)
