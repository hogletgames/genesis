function(ge_build_mono)
    cmake_parse_arguments(GE_MONO "" "" "SOURCE_DIR;INSTALL_DIR" ${ARGN})

    if (NOT GE_MONO_SOURCE_DIR)
        message(FATAL_ERROR "SOURCE_DIR is not set")
    endif ()

    if (NOT GE_MONO_INSTALL_DIR)
        message(FATAL_ERROR "INSTALL_DIR is not set")
    endif ()

    cmake_host_system_information(RESULT GE_NPROC QUERY NUMBER_OF_LOGICAL_CORES)

    # Configure mono
    execute_process(
        COMMAND ./autogen.sh --prefix=${GE_MONO_INSTALL_DIR}
        WORKING_DIRECTORY ${mono_SOURCE_DIR}
        RESULT_VARIABLE result
    )
    if (result)
        message(FATAL_ERROR "Configuration of Mono failed")
    endif ()

    # Fetch mono runtime
    execute_process(
        COMMAND make get-monolite-latest
        WORKING_DIRECTORY ${GE_MONO_SOURCE_DIR}
        RESULT_VARIABLE result
    )
    if (result)
        message(FATAL_ERROR "Fetching of Mono Runtime failed")
    endif ()

    # Build mono
    execute_process(
        COMMAND make -j${GE_NPROC}
        WORKING_DIRECTORY ${GE_MONO_SOURCE_DIR}
        RESULT_VARIABLE result
    )
    if (result)
        message(FATAL_ERROR "Building Mono failed")
    endif ()

    # Install mono
    execute_process(
        COMMAND make install
        WORKING_DIRECTORY ${GE_MONO_SOURCE_DIR}
        RESULT_VARIABLE result
    )
    if (result)
        message(FATAL_ERROR "Installing Mono failed")
    endif ()
endfunction()

function(ge_mono_create_assembly TARGET)
    cmake_parse_arguments(GE_MONO "" "FILEPATH" "SOURCES" ${ARGN})

    set(WORKING_DIR "${CMAKE_CURRENT_LIST_DIR}")

    if (NOT GE_MONO_SOURCES)
        message(FATAL_ERROR "No sources provided")
    endif ()

    if (GE_MONO_FILEPATH)
        set(GE_MONO_ASSEMBLY ${GE_MONO_FILEPATH})
    else ()
        set(GE_MONO_ASSEMBLY ${CMAKE_CURRENT_BINARY_DIR}/${TARGET})
    endif ()

    if (NOT GE_MONO_MCS)
        include(FetchContent)
        FetchContent_GetProperties(mono BINARY_DIR GE_MONO_BINARY_DIR)

        find_file(GE_MONO_MCS
            NAMES mcs
            PATHS ${GE_MONO_BINARY_DIR}/bin
            REQUIRED
            NO_DEFAULT_PATH
        )

        set(GE_MONO_MCS "${GE_MONO_MCS}" CACHE FILEPATH "Path to mcs binary" FORCE)
    endif ()

    add_custom_command(
        OUTPUT ${GE_MONO_ASSEMBLY}
        COMMAND ${GE_MONO_MCS} -target:library -r:Mono.CSharp.dll -out:${GE_MONO_ASSEMBLY} ${GE_MONO_SOURCES}
        WORKING_DIRECTORY ${WORKING_DIR}
        BYPRODUCTS ${GE_MONO_ASSEMBLY}
        DEPENDS ${GE_MONO_SOURCES}
        COMMENT "Creating assembly ${GE_MONO_ASSEMBLY}"
    )

    add_custom_target(${TARGET} DEPENDS ${GE_MONO_ASSEMBLY})
endfunction()
