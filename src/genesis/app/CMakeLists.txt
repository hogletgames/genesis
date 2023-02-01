set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/app)

list(APPEND APP_SOURCES
    application.cpp)

list(APPEND APP_HEADERS
    ${INCLUDE_DIR}/application.h
    ${INCLUDE_DIR}/layer.h)

ge_add_module(app
    SOURCES ${APP_SOURCES} ${APP_SOURCES}
    INCLUDE_DIRS ${INCLUDE_DIR}
    PUBLIC_DEPS
        genesis::core
        genesis::window)
