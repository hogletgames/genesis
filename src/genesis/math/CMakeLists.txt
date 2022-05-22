set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/math)

list(APPEND MATH_SOURCES
    types.cpp)

list(APPEND MATH_HEADERS
    ${INCLUDE_DIR}/types.h)

ge_add_module(math
    SOURCES ${MATH_SOURCES} ${MATH_HEADERS}
    INCLUDE_DIRS ${INCLUDE_DIR}
    PUBLIC_DEPS
        genesis::core
        glm::glm)
