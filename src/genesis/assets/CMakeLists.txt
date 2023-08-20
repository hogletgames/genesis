set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/assets)

list(APPEND ASSETS_SOURCES
    resource_base.cpp
    resource_traversal.cpp
    )

list(APPEND ASSETS_HEADERS
    ${INCLUDE_DIR}/iresource.h
    ${INCLUDE_DIR}/resource_base.h
    ${INCLUDE_DIR}/resource_id.h
    ${INCLUDE_DIR}/resource_pointer_visitor.h
    ${INCLUDE_DIR}/resource_traversal.h
    ${INCLUDE_DIR}/resource_visitor.h
    )

ge_add_module(assets
    SOURCES ${ASSETS_SOURCES} ${ASSETS_HEADERS}
    INCLUDE_DIRS ${INCLUDE_DIR}
    PUBLIC_DEPS
        genesis::core
    PRIVATE_DEPS
        Boost::uuid
    )
