set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/script)

list(APPEND SCRIPT_SOURCES
    host_framework.cpp
    )

list(APPEND SCRIPT_HEADERS
    ${INCLUDE_DIR}/delegate_loader.h
    ${INCLUDE_DIR}/host_framework.h
    )

ge_add_module(script
    SOURCES ${SCRIPT_SOURCES} ${SCRIPT_HEADERS}
    INCLUDE_DIRS ${INCLUDE_DIR}
    PUBLIC_DEPS
        genesis::core
        nethost
    PRIVATE_DEPS
        genesis::dll
    )
