set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/assets)

list(APPEND ASSETS_SOURCES
    )

list(APPEND ASSETS_HEADERS
    ${INCLUDE_DIR}/resource_id.h
    )

ge_add_module(assets
    SOURCES ${ASSETS_SOURCES} ${ASSETS_HEADERS}
    INCLUDE_DIRS ${INCLUDE_DIR}
    PUBLIC_DEPS
        genesis::core
    PRIVATE_DEPS
        Boost::uuid
    )
