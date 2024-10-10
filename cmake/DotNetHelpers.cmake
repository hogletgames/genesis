find_package(ge-dotnet REQUIRED)

function(ge_dotnet_build CSPROJ_FILE)
    cmake_parse_arguments(GE_DOTNET "" "TARGET;OUTPUT" "" ${ARGN})

    get_filename_component(CSPROJ_FILE ${CSPROJ_FILE} ABSOLUTE)

    if (NOT EXISTS ${CSPROJ_FILE})
        message(FATAL_ERROR "Project file '${PROJECT_FILE}' does not exist")
    endif ()

    get_filename_component(DOTNET_PROJECT_NAME ${CSPROJ_FILE} NAME_WE)
    get_filename_component(DOTNET_PROJECT_DIR ${CSPROJ_FILE} DIRECTORY)

    if (NOT GE_DOTNET_TARGET)
        set(GE_DOTNET_TARGET ${DOTNET_PROJECT_NAME})
    endif ()

    if (NOT GE_DOTNET_OUTPUT)
        set(GE_DOTNET_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${DOTNET_PROJECT_NAME})
    endif ()

    add_custom_command(
        OUTPUT ${GE_DOTNET_OUTPUT}
        COMMAND dotnet format ${CSPROJ_FILE}
        COMMAND dotnet build ${CSPROJ_FILE} --output ${GE_DOTNET_OUTPUT}
        BYPRODUCTS ${GE_DOTNET_OUTPUT}
        DEPENDS ${DOTNET_PROJECT_DIR}
        COMMENT "Building '${DOTNET_PROJECT_NAME}'..."
        )

    add_custom_target(${GE_DOTNET_TARGET} DEPENDS ${GE_DOTNET_OUTPUT})
endfunction()
