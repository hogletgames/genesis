set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/filesystem)

list(APPEND FS_SOURCES
    filepath.cpp
    tmp_dir_guard.cpp
    )

list(APPEND FS_HEADERS
    ${INCLUDE_DIR}/file_content.h
    ${INCLUDE_DIR}/filepath.h
    ${INCLUDE_DIR}/tmp_dir_guard.h
    )

ge_add_module(filesystem
    SOURCES ${FS_SOURCES} ${FS_HEADERS}
    INCLUDE_DIRS ${INCLUDE_DIR}
    PUBLIC_DEPS
        genesis::core
    PRIVATE_DEPS
        Boost::filesystem
    )
