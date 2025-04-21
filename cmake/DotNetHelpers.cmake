find_package(ge-dotnet REQUIRED)

# ge_dotnet_build(DOTNET_PROJECT_FILE
# [ONE_VALUE_KEYWORDS]
#   TARGET <target name>
#   OUTPUT <output directory>
# )
# This file provides helper functions for building .NET projects using CMake.
# It allows you to specify a .NET project file and build it, as well as format the
# project files using the `dotnet format` command.
# Usage:
# #   ge_dotnet_build(MyProject.csproj
#       TARGET MyProject
#       OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/MyProject
#   )

function(ge_dotnet_build DOTNET_PROJECT_FILE)
    cmake_parse_arguments(THIS "" "TARGET;OUTPUT" "" ${ARGN})

    get_filename_component(DOTNET_PROJECT_FILE ${DOTNET_PROJECT_FILE} ABSOLUTE)

    if (NOT EXISTS ${DOTNET_PROJECT_FILE})
        message(FATAL_ERROR "Project file '${PROJECT_FILE}' does not exist")
    endif ()

    get_filename_component(DOTNET_PROJECT_NAME ${DOTNET_PROJECT_FILE} NAME_WE)
    get_filename_component(DOTNET_PROJECT_DIR ${DOTNET_PROJECT_FILE} DIRECTORY)
    file(GLOB_RECURSE SOURCE_FILES "${DOTNET_PROJECT_DIR}/*.cs")

    if (NOT THIS_TARGET)
        set(THIS_TARGET ${DOTNET_PROJECT_NAME})
    endif ()

    if (NOT THIS_OUTPUT)
        set(THIS_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${THIS_TARGET})
    endif ()

    set(OUTPUT_DLL ${THIS_OUTPUT}/${DOTNET_PROJECT_NAME}.dll)

    # Build project target
    add_custom_command(
        OUTPUT ${THIS_OUTPUT}
        COMMAND dotnet build ${DOTNET_PROJECT_FILE}
                    --configuration Release
                    --artifacts-path ${THIS_OUTPUT}/obj
                    --output ${THIS_OUTPUT}
                    --disable-build-servers
        WORKING_DIRECTORY ${DOTNET_PROJECT_DIR}
        DEPENDS ${SOURCE_FILES} ${DOTNET_PROJECT_FILE}
        COMMENT "Building '${THIS_TARGET}'..."
        VERBATIM
        )
    add_custom_target(${THIS_TARGET} DEPENDS ${THIS_OUTPUT})
    set(${THIS_TARGET}_BINARY_DIR ${THIS_OUTPUT} CACHE FILEPATH
        "${THIS_TARGET} output directory" FORCE)

    # Format project target
    add_custom_target(${THIS_TARGET}_format
        COMMAND dotnet format style ${DOTNET_PROJECT_FILE}
                    --verbosity detailed
        WORKING_DIRECTORY ${DOTNET_PROJECT_DIR}
        DEPENDS ${SOURCE_FILES}
        COMMENT "Formatting '${THIS_TARGET}' files..."
        )
endfunction()
