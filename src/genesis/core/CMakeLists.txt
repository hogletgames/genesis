set(GE_CORE_INCLUDE_DIR ${GE_INCLUDE_DIR}/core)

list(APPEND GE_CORE_HEADERS
    ${GE_CORE_INCLUDE_DIR}/export.h
)

add_library(genesis-core STATIC ${GE_CORE_SRC} ${GE_CORE_HEADERS})
target_include_directories(genesis-core PRIVATE ${GE_CORE_INCLUDE_DIR})
add_library(genesis::core ALIAS genesis-core)
