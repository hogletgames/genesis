set(GE_APP_INCLUDE_DIR ${GE_INCLUDE_DIR}/app)

list(APPEND GE_CORE_SRC
    application.cpp)

list(APPEND GE_CORE_HEADERS
    ${GE_APP_INCLUDE_DIR}/application.h
    ${GE_APP_INCLUDE_DIR}/layer.h)

add_library(genesis-app STATIC ${GE_CORE_SRC} ${GE_CORE_HEADERS})
target_link_libraries(genesis-app PUBLIC
    genesis::core
    genesis::window)
target_include_directories(genesis-app PRIVATE ${GE_APP_INCLUDE_DIR})
ge_default_target_config(genesis-app)
add_library(genesis::app ALIAS genesis-app)
