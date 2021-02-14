set(GE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/genesis)

add_definitions(-DGE_EXPORTS)

list(APPEND GE_HEADERS
    ${GE_INCLUDE_DIR}/core.h
    ${GE_INCLUDE_DIR}/genesis.h
    ${GE_INCLUDE_DIR}/math.h
)

add_library(genesis ${GE_LIB_TYPE} ${GE_HEADERS})
target_link_libraries(genesis PUBLIC
    genesis::core
    genesis::math
)
add_library(genesis::genesis ALIAS genesis)

add_subdirectory(core)
add_subdirectory(math)
