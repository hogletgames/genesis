set(GE_WINDOW_INCLUDE_DIR ${GE_INCLUDE_DIR}/window)

list(APPEND GE_WINDOW_SCR
    window.cpp
)

list(APPEND GE_WINDOW_HEADERS
    ${GE_WINDOW_INCLUDE_DIR}/window.h
)

add_library(genesis-window STATIC ${GE_WINDOW_SCR} ${GE_WINDOW_HEADERS})
target_link_libraries(genesis-window PUBLIC
    genesis::core
    genesis::math
    genesis::window::platform
)
target_include_directories(genesis-window PRIVATE ${GE_WINDOW_INCLUDE_DIR})
ge_default_target_config(genesis-window)
add_library(genesis::window ALIAS genesis-window)

add_subdirectory(SDL)
