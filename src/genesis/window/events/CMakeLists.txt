set(GE_WINDOW_EVENTS_INCLUDE_DIR ${GE_WINDOW_INCLUDE_DIR}/events)

list(APPEND GE_WINDOW_EVENTS_HEADERS
    ${GE_WINDOW_EVENTS_INCLUDE_DIR}/event.h
    ${GE_WINDOW_EVENTS_INCLUDE_DIR}/event_dispatcher.h)

add_library(genesis-window-events INTERFACE)
target_link_libraries(genesis-window-events INTERFACE genesis::core)
add_library(genesis::window::events ALIAS genesis-window-events)
