set(GE_WINDOW_EVENTS_INCLUDE_DIR ${GE_WINDOW_INCLUDE_DIR}/events)

list(APPEND GE_WINDOW_EVENTS_SRC
    key_events.cpp
    mouse_events.cpp)

list(APPEND GE_WINDOW_EVENTS_HEADERS
    ${GE_WINDOW_EVENTS_INCLUDE_DIR}/event.h
    ${GE_WINDOW_EVENTS_INCLUDE_DIR}/event_dispatcher.h
    ${GE_WINDOW_EVENTS_INCLUDE_DIR}/key_events.h
    ${GE_WINDOW_EVENTS_INCLUDE_DIR}/mouse_events.h)

add_library(genesis-window-events STATIC ${GE_WINDOW_EVENTS_SRC} ${GE_WINDOW_EVENTS_HEADERS})
target_link_libraries(genesis-window-events PUBLIC
    genesis::core
    genesis::math)
target_include_directories(genesis-window-events PRIVATE ${GE_WINDOW_EVENTS_INCLUDE_DIR})
ge_default_target_config(genesis-window-events)
add_library(genesis::window::events ALIAS genesis-window-events)
