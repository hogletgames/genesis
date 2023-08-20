set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/assets)

list(APPEND ASSETS_SOURCES
    mesh_resource.cpp
    pipeline_resource.cpp
    registry.cpp
    resource_base.cpp
    resource_deserializer.cpp
    resource_serializer.cpp
    resource_traversal.cpp
    texture_resource.cpp
    )

list(APPEND ASSETS_HEADERS
    ${INCLUDE_DIR}/assets_exception.h
    ${INCLUDE_DIR}/iresource.h
    ${INCLUDE_DIR}/mesh_resource.h
    ${INCLUDE_DIR}/pipeline_resource.h
    ${INCLUDE_DIR}/registry.h
    ${INCLUDE_DIR}/resource_base.h
    ${INCLUDE_DIR}/resource_deserializer.h
    ${INCLUDE_DIR}/resource_id.h
    ${INCLUDE_DIR}/resource_pointer_visitor.h
    ${INCLUDE_DIR}/resource_serializer.h
    ${INCLUDE_DIR}/resource_traversal.h
    ${INCLUDE_DIR}/resource_visitor.h
    ${INCLUDE_DIR}/texture_resource.h
    ${INCLUDE_DIR}/yaml_convert.h
    )

ge_add_module(assets
    SOURCES ${ASSETS_SOURCES} ${ASSETS_HEADERS}
    INCLUDE_DIRS ${INCLUDE_DIR}
    PUBLIC_DEPS
        genesis::core
        genesis::graphics
        yaml-cpp::yaml-cpp
    PRIVATE_DEPS
        Boost::uuid
    )
