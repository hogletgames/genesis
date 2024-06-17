set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/math)

list(APPEND MATH_SOURCES
    types.cpp
    )

list(APPEND MATH_HEADERS
    ${INCLUDE_DIR}/camera.h
    ${INCLUDE_DIR}/linear.h
    ${INCLUDE_DIR}/transform.h
    ${INCLUDE_DIR}/trigonometric.h
    ${INCLUDE_DIR}/types.h
    ${INCLUDE_DIR}/yaml_convert.h
    )

ge_add_module(math
    SOURCES ${MATH_SOURCES} ${MATH_HEADERS}
    INCLUDE_DIRS ${INCLUDE_DIR}
    PUBLIC_DEPS
        genesis::core
        glm::glm
        yaml-cpp::yaml-cpp
    PUBLIC_DEF
        GLM_ENABLE_EXPERIMENTAL
        GLM_FORCE_DEPTH_ZERO_TO_ONE
        GLM_FORCE_RADIANS
    )
