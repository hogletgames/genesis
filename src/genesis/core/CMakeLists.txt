set(GE_CORE_INCLUDE_DIR ${GE_INCLUDE_DIR}/core)

list(APPEND GE_CORE_SRC
    log.cpp
)

list(APPEND GE_CORE_HEADERS
    ${GE_CORE_INCLUDE_DIR}/asserts.h
    ${GE_CORE_INCLUDE_DIR}/export.h
    ${GE_CORE_INCLUDE_DIR}/log.h
    ${GE_CORE_INCLUDE_DIR}/memory.h
    ${GE_CORE_INCLUDE_DIR}/utils.h
)

add_library(genesis-core STATIC ${GE_CORE_SRC} ${GE_CORE_HEADERS})
target_link_libraries(genesis-core PUBLIC spdlog)
target_include_directories(genesis-core SYSTEM PUBLIC
    ${GE_THIRD_PARTY_DIR}/spdlog/include
)
target_include_directories(genesis-core PRIVATE ${GE_CORE_INCLUDE_DIR})
set_target_properties(genesis-core
    PROPERTIES POSITION_INDEPENDENT_CODE ON
)
add_library(genesis::core ALIAS genesis-core)
