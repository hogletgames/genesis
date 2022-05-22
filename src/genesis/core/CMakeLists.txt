set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/core)

list(APPEND CORE_SOURCES
    log.cpp)

list(APPEND CORE_HEADERS
    ${INCLUDE_DIR}/asserts.h
    ${INCLUDE_DIR}/defer.h
    ${INCLUDE_DIR}/enum.h
    ${INCLUDE_DIR}/exception.h
    ${INCLUDE_DIR}/export.h
    ${INCLUDE_DIR}/filesystem.h
    ${INCLUDE_DIR}/format.h
    ${INCLUDE_DIR}/interface.h
    ${INCLUDE_DIR}/log.h
    ${INCLUDE_DIR}/memory.h
    ${INCLUDE_DIR}/timestamp.h
    ${INCLUDE_DIR}/utils.h)

ge_add_module(core
    SOURCES ${CORE_SOURCES} ${CORE_HEADERS}
    INCLUDE_DIRS ${INCLUDE_DIR}
    PUBLIC_DEPS
        fmt::fmt
        magic_enum::magic_enum
        spdlog::spdlog)
