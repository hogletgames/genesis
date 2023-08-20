include(${PROJECT_SOURCE_DIR}/cmake/Macro.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/CompilerOptions.cmake)

set(GE_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(GE_CONFIGURABLE_FILES_DEST "${PROJECT_BINARY_DIR}/configurable-files")

function(ge_add_module MODULE)
    set(TARGET genesis-${MODULE})
    _ge_parse_add_target_options(${ARGN})
    add_library(${TARGET} STATIC ${THIS_SOURCES})
    _ge_set_target_options(${TARGET})
    _ge_configure_files(${MODULE} ${TARGET})
    add_library(genesis::${MODULE} ALIAS ${TARGET})
endfunction()

function(ge_add_executable TARGET)
    _ge_parse_add_target_options(${ARGN})
    add_executable(${TARGET} ${THIS_SOURCES})
    _ge_set_target_options(${TARGET})
endfunction()

macro(_ge_parse_add_target_options)
    cmake_parse_arguments(THIS
        ""
        ""
        "INCLUDE_DIRS;SOURCES;PUBLIC_DEPS;PRIVATE_DEPS;PUBLIC_DEF;PRIVATE_DEF"
        ${ARGN})
endmacro()

function(_ge_set_target_options TARGET)
    # Module include directory
    target_include_directories(${TARGET}
        PUBLIC
            ${GE_INCLUDE_DIR}
            ${GE_CONFIGURABLE_FILES_DEST}
        PRIVATE
            ${THIS_INCLUDE_DIRS})

    # Dependencies
    target_link_libraries(${TARGET} PUBLIC ${THIS_PUBLIC_DEPS} PRIVATE ${THIS_PRIVATE_DEPS})

    # Compiler options
    target_compile_features(${TARGET} PRIVATE cxx_std_17)
    target_compile_options(${TARGET} PRIVATE ${GE_COMPILE_OPTIONS})

    # Definitions
    target_compile_definitions(${TARGET}
        PUBLIC
            ${THIS_PUBLIC_DEF}
            $<$<BOOL:${GE_DISABLE_ASSERTS}>:GE_DISABLE_ASSERTS>
            $<$<BOOL:${GE_DISABLE_DEBUG}>:GE_DISABLE_DEBUG>
        PRIVATE
            ${THIS_PRIVATE_DEF}
        )
    _ge_set_target_platform(${TARGET})
endfunction()

function(_ge_set_target_platform TARGET)
    if (UNIX)
        target_compile_definitions(${TARGET} PUBLIC GE_PLATFORM_UNIX)
    elseif (WIN32)
        target_compile_definitions(${TARGET} PUBLIC GE_PLATFORM_WINDOWS)
    endif()

    if (APPLE)
        target_compile_definitions(${TARGET} PUBLIC GE_PLATFORM_APPLE)
    endif ()
endfunction()

function(_ge_configure_files MODULE TARGET)
    set(CONFIG_DIR_SRC "${PROJECT_SOURCE_DIR}/cmake/configurable-files/${MODULE}")
    set(CONFIG_DIR_DEST "${GE_CONFIGURABLE_FILES_DEST}/genesis/${MODULE}")

    if (NOT EXISTS ${CONFIG_DIR_SRC})
        return()
    endif ()

    file(GLOB CONFIGURABLE_FILES ${CONFIG_DIR_SRC}/*.in)

    foreach (SRC_FILEPATH IN LISTS CONFIGURABLE_FILES)
        get_filename_component(SRC_FILENAME "${SRC_FILEPATH}" NAME)
        string(REGEX REPLACE "\.in$" "" DEST_FILENAME "${SRC_FILENAME}")
        set(DEST_FILEPATH "${CONFIG_DIR_DEST}/${DEST_FILENAME}")
        configure_file("${SRC_FILEPATH}" "${DEST_FILEPATH}" @ONLY)
    endforeach ()

    target_include_directories(${TARGET} PRIVATE ${CONFIG_DIR_DEST})
endfunction()
