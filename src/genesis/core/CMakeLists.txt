set(GE_CORE_INCLUDE_DIR ${GE_INCLUDE_DIR}/core)

list(APPEND GE_CORE_SRC log.cpp)

list(APPEND GE_CORE_HEADERS
    ${GE_CORE_INCLUDE_DIR}/asserts.h
    ${GE_CORE_INCLUDE_DIR}/defer.h
    ${GE_CORE_INCLUDE_DIR}/enum.h
    ${GE_CORE_INCLUDE_DIR}/exception.h
    ${GE_CORE_INCLUDE_DIR}/export.h
    ${GE_CORE_INCLUDE_DIR}/format.h
    ${GE_CORE_INCLUDE_DIR}/interface.h
    ${GE_CORE_INCLUDE_DIR}/log.h
    ${GE_CORE_INCLUDE_DIR}/memory.h
    ${GE_CORE_INCLUDE_DIR}/timestamp.h
    ${GE_CORE_INCLUDE_DIR}/utils.h
    ${GE_CORE_INCLUDE_DIR}/version.h)

add_library(genesis-core STATIC ${GE_CORE_SRC} ${GE_CORE_HEADERS})
target_link_libraries(genesis-core PUBLIC
    fmt
    magic_enum
    spdlog)
target_include_directories(genesis-core SYSTEM PUBLIC
    ${GE_THIRD_PARTY_DIR}/fmt/include
    ${GE_THIRD_PARTY_DIR}/magic_enum/include
    ${GE_THIRD_PARTY_DIR}/spdlog/include)
target_include_directories(genesis-core PRIVATE ${GE_CORE_INCLUDE_DIR})
target_compile_definitions(genesis-core PUBLIC
    -D_GE_VER_MAJOR=${CMAKE_PROJECT_VERSION_MAJOR}
    -D_GE_VER_MINOR=${CMAKE_PROJECT_VERSION_MINOR}
    -D_GE_VER_PATCH=${CMAKE_PROJECT_VERSION_PATCH})
set_target_properties(genesis-core PROPERTIES POSITION_INDEPENDENT_CODE ON)
ge_default_target_config(genesis-core)
add_library(genesis::core ALIAS genesis-core)
