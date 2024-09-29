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
        find_file(GE_MONO_MCS
            NAMES mcs
            HINTS ${MONO_ROOT}
            PATH_SUFFIXES bin
            REQUIRED
            )

        set(GE_MONO_MCS "${GE_MONO_MCS}" CACHE FILEPATH "Path to mcs binary" FORCE)
    endif ()

    add_custom_command(
        OUTPUT ${GE_MONO_ASSEMBLY}
        COMMAND ${GE_MONO_MCS} -target:library
                               -r:Mono.CSharp.dll
                               -out:${GE_MONO_ASSEMBLY}
                               ${GE_MONO_SOURCES}
        WORKING_DIRECTORY ${WORKING_DIR}
        BYPRODUCTS ${GE_MONO_ASSEMBLY}
        DEPENDS ${GE_MONO_SOURCES}
        COMMENT "Creating assembly ${GE_MONO_ASSEMBLY}"
        )

    add_custom_target(${TARGET} DEPENDS ${GE_MONO_ASSEMBLY})
endfunction()
