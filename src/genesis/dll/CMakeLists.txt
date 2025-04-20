set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/dll)

list(APPEND DLL_SOURCES
    path.cpp
    shared_library.cpp
    )

if (APPLE)
    list(APPEND DLL_SOURCES
        platform/path_apple.cpp
        platform/shared_library_unix.cpp
    )
elseif (UNIX)
    list(APPEND DLL_SOURCES
        platform/path_unix.cpp
        platform/shared_library_unix.cpp
    )
else ()
    message(FATAL_ERROR "Unsupported platform")
endif ()

list(APPEND DLL_HEADERS
    ${INCLUDE_DIR}/path.h
    ${INCLUDE_DIR}/shared_library.h
    platform/path.h
    platform/shared_library.h
    )

ge_add_module(dll
    SOURCES ${DLL_SOURCES} ${DLL_HEADERS}
    INCLUDE_DIRS ${INCLUDE_DIR}
    PUBLIC_DEPS
        genesis::core
    PRIVATE_DEPS
        genesis::filesystem
    )
