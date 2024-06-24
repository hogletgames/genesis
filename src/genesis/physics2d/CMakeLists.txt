set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/physics2d)

list(APPEND FHYSICS2D_SOURCES
    world.cpp
    )

list(APPEND FHYSICS2D_HEADERS
    ${INCLUDE_DIR}/rigid_body.h
    ${INCLUDE_DIR}/rigid_body_shape.h
    ${INCLUDE_DIR}/world.h
    )

ge_add_module(physics2d
    SOURCES ${FHYSICS2D_SOURCES} ${FHYSICS2D_HEADERS}
    INCLUDE_DIRS ${INCLUDE_DIR}
    PRIVATE_DEPS
        genesis::math
        genesis::physics2d-box2d
    )

add_subdirectory(b2d)
