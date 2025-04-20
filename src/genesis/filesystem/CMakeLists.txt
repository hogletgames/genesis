set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/filesystem)

list(APPEND FS_SOURCES
    file.cpp
    known_folders.cpp
    tmp_dir_guard.cpp
    )

list(APPEND FS_HEADERS
    ${INCLUDE_DIR}/file.h
    ${INCLUDE_DIR}/file_content.h
    ${INCLUDE_DIR}/filepath.h
    ${INCLUDE_DIR}/known_folders.h
    ${INCLUDE_DIR}/tmp_dir_guard.h
    platform/known_folders.h
    )

if (APPLE)
    list(APPEND FS_SOURCES
        platform/known_folders_apple.cpp
        )
elseif (UNIX)
    list(APPEND FS_SOURCES
        platform/known_folders_unix.cpp
        )
elseif (WIN32)
    list (APPEND FS_SOURCES
        platform/known_folders_windows.cpp
        )
endif()

ge_add_module(filesystem
    SOURCES ${FS_SOURCES} ${FS_HEADERS}
    INCLUDE_DIRS ${INCLUDE_DIR}
    PUBLIC_DEPS
        genesis::core
    PRIVATE_DEPS
        Boost::filesystem
    )
