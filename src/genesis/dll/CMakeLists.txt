set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/dll)

list(APPEND DLL_SOURCES
    shared_library.cpp
    )

if (APPLE)
    list(APPEND DLL_SOURCES
        platform/shared_library_unix.cpp
    )
elseif (UNIX)
    list(APPEND DLL_SOURCES
        platform/shared_library_unix.cpp
    )
else ()
    message(FATAL_ERROR "Unsupported platform")
endif ()

list(APPEND DLL_HEADERS
    ${INCLUDE_DIR}/shared_library.h
    platform/shared_library.h
    )

ge_add_module(dll
    SOURCES ${DLL_SOURCES} ${DLL_HEADERS}
    INCLUDE_DIRS ${INCLUDE_DIR}
    PUBLIC_DEPS
        genesis::core
    )
