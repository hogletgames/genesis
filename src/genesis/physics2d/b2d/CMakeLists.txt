list(APPEND FHYSICS2D_BOX2D_SOURCES
    world.cpp
    )

list(APPEND FHYSICS2D_BOX2D_HEADERS
    math_types.h
    world.h
    )

ge_add_module(physics2d-box2d
    SOURCES ${FHYSICS2D_BOX2D_SOURCES} ${FHYSICS2D_BOX2D_HEADERS}
    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC_DEPS
        box2d
        genesis::math
    )
