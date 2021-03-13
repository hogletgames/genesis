set(GE_APP_INCLUDE_DIR ${GE_INCLUDE_DIR}/app)

list(APPEND GE_CORE_HEADERS
    ${GE_APP_INCLUDE_DIR}/layer.h)

add_library(genesis-app INTERFACE)
target_link_libraries(genesis-app INTERFACE genesis::core)
add_library(genesis::app ALIAS genesis-app)
