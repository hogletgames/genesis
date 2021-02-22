set(GE_MATH_INCLUDE_DIR ${GE_INCLUDE_DIR}/math)

list(APPEND GE_MATH_HEADERS
    ${GE_MATH_INCLUDE_DIR}/math.h
)

add_library(genesis-math INTERFACE)
target_link_libraries(genesis-math INTERFACE glm::glm)
target_include_directories(genesis-math SYSTEM INTERFACE
    ${GE_THIRD_PARTY_DIR}/glm
)
add_library(genesis::math ALIAS genesis-math)
