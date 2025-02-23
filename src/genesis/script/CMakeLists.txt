include(FindPkgConfig)
pkg_check_modules(Mono REQUIRED IMPORTED_TARGET mono-2)

set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/script)

list(APPEND SCRIPT_HEADERS
    ${INCLUDE_DIR}/assembly.h
    ${INCLUDE_DIR}/bittable_type.h
    ${INCLUDE_DIR}/class.h
    ${INCLUDE_DIR}/class_type.h
    ${INCLUDE_DIR}/class_type_traits.h
    ${INCLUDE_DIR}/concepts.h
    ${INCLUDE_DIR}/domain.h
    ${INCLUDE_DIR}/invoke_result.h
    ${INCLUDE_DIR}/method.h
    ${INCLUDE_DIR}/object.h
    ${INCLUDE_DIR}/scripting_engine.h
    ${INCLUDE_DIR}/string_type.h
    ${INCLUDE_DIR}/type_traits.h
    )

list(APPEND SCRIPT_SOURCES
    assembly.cpp
    bittable_type.cpp
    class.cpp
    class_type.cpp
    domain.cpp
    invoke_result.cpp
    method.cpp
    object.cpp
    scripting_engine.cpp
    string_type.cpp
    )

ge_add_module(script
    SOURCES ${SCRIPT_SOURCES} ${SCRIPT_HEADERS}
    INCLUDE_DIRS ${INCLUDE_DIR}
    PUBLIC_DEPS
        genesis::core
    PRIVATE_DEPS
        PkgConfig::Mono
    )
