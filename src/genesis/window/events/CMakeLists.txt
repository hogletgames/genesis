set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/window/events)

list(APPEND WINDOW_EVENTS_SOURCES
    key_events.cpp
    mouse_events.cpp
    window_events.cpp)

list(APPEND WINDOW_EVENTS_HEADERS
    ${INCLUDE_DIR}/event.h
    ${INCLUDE_DIR}/event_dispatcher.h
    ${INCLUDE_DIR}/key_events.h
    ${INCLUDE_DIR}/mouse_events.h
    ${INCLUDE_DIR}/window_events.h)

ge_add_module(window-events
    SOURCES ${WINDOW_EVENTS_SOURCES} ${WINDOW_EVENTS_HEADERS}
    INCLUDE_DIR ${INCLUDE_DIR}
    PUBLIC_DEPS
        genesis::core
        genesis::math)
