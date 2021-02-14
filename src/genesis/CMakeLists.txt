set(GE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/genesis)

add_definitions(-DGE_EXPORTS)

list(APPEND GE_HEADERS
    ${GE_INCLUDE_DIR}/core.h
    ${GE_INCLUDE_DIR}/genesis.h
)

add_library(genesis ${GE_LIB_TYPE} ${GE_HEADERS})
target_link_libraries(genesis PUBLIC
    genesis::core
)
add_library(genesis::genesis ALIAS genesis)

add_subdirectory(core)
