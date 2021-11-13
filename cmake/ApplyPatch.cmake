function(ge_apply_patch REPO_DIR)
    # Full path to patched repo
    get_filename_component(ORIGIN_REPO_REALPATH ${REPO_DIR} ABSOLUTE)

    # Path to patch dir for the repo
    get_filename_component(REPO_NAME ${ORIGIN_REPO_REALPATH} NAME)
    set(PATCH_DIR ${GE_THIRD_PARTY_DIR}/patch/${REPO_NAME})

    message(STATUS "Patching '${REPO_NAME}':")
    file(GLOB PATCH_FILES "${PATCH_DIR}/*.patch")

    if (NOT PATCH_FILES)
        message(SEND_ERROR "There is no patches in '${PATCH_DIR}'")
        return()
    endif ()

    foreach (PATCH ${PATCH_FILES})
        get_filename_component(PATCH_FILENAME ${PATCH} NAME)

        # Trying to apply a patch
        execute_process(COMMAND patch -p 1 --forward --ignore-whitespace
            WORKING_DIRECTORY ${ORIGIN_REPO_REALPATH}
            INPUT_FILE ${PATCH}
            ERROR_VARIABLE ERROR_OUTPUT
            RESULT_VARIABLE PATCH_RESULT
            OUTPUT_QUIET)

        if (PATCH_RESULT EQUAL 0)
            message(STATUS "  - Applying '${PATCH_FILENAME}'... OK")
            continue()
        endif ()

        # If the patch has already been applied 'patch' will return an error.
        # Trying to ensure that patch was applied.
        execute_process(COMMAND patch -p 1 -R --dry-run
            WORKING_DIRECTORY ${ORIGIN_REPO_REALPATH}
            INPUT_FILE ${PATCH}
            ERROR_VARIABLE ERROR_OUTPUT
            RESULT_VARIABLE PATCH_RESULT
            OUTPUT_QUIET)

        if (PATCH_RESULT EQUAL 0)
            message(STATUS "  - Applying '${PATCH_FILENAME}'... OK")
        else ()
            string(STRIP ${ERROR_OUTPUT} STRIPPED_ERROR_OUTPUT)
            message(SEND_ERROR "  - Applying '${PATCH_FILENAME}'... FAILED")
        endif ()
    endforeach ()
endfunction()
