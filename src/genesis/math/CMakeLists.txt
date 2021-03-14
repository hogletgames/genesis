set(GE_MATH_INCLUDE_DIR ${GE_INCLUDE_DIR}/math)

list(APPEND GE_MATH_SRC
    types.cpp)

list(APPEND GE_MATH_HEADERS
    ${GE_MATH_INCLUDE_DIR}/types.h)

add_library(genesis-math ${GE_MATH_SRC} ${GE_MATH_HEADERS})
target_link_libraries(genesis-math PUBLIC
    genesis::core
    glm)
target_include_directories(genesis-math SYSTEM PUBLIC ${GE_THIRD_PARTY_DIR}/glm)
target_include_directories(genesis-math PRIVATE ${GE_MATH_INCLUDE_DIR})
ge_default_target_config(genesis-math)
add_library(genesis::math ALIAS genesis-math)
