include(FindPkgConfig)
pkg_check_modules(Mono REQUIRED IMPORTED_TARGET mono-2)

set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/script)

list(APPEND SCRIPT_HEADERS
    ${INCLUDE_DIR}/class_type.h
    ${INCLUDE_DIR}/class_type_traits.h
    ${INCLUDE_DIR}/type_traits.h
    )

ge_add_module(script INTERFACE
    SOURCES ${SCRIPT_HEADERS}
    INCLUDE_DIRS ${INCLUDE_DIR}
    PUBLIC_DEPS
        genesis::core
    PRIVATE_DEPS
        PkgConfig::Mono
    )
