find_package(ge-dotnet REQUIRED)

function(ge_dotnet_build TARGET CSPROJ_FILE)
    cmake_parse_arguments(GE_DOTNET "" "OUTPUT" "" ${ARGN})

    get_filename_component(CSPROJ_FILE ${CSPROJ_FILE} ABSOLUTE)

    if (NOT EXISTS ${CSPROJ_FILE})
        message(FATAL_ERROR "Project file '${PROJECT_FILE}' does not exist")
    endif ()

    get_filename_component(DOTNET_PROJECT_NAME ${CSPROJ_FILE} NAME_WE)
    get_filename_component(DOTNET_PROJECT_DIR ${CSPROJ_FILE} DIRECTORY)

    if (NOT GE_DOTNET_OUTPUT)
        set(GE_DOTNET_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${DOTNET_PROJECT_NAME})
    endif ()

    set(OUTPUT_DLL ${GE_DOTNET_OUTPUT}/${DOTNET_PROJECT_NAME}.dll)

    add_custom_command(
        OUTPUT ${OUTPUT_DLL}
        COMMAND dotnet format ${CSPROJ_FILE}
        COMMAND dotnet build -c Release ${CSPROJ_FILE} --output ${GE_DOTNET_OUTPUT}
        BYPRODUCTS ${OUTPUT_DLL}
        COMMENT "Building '${DOTNET_PROJECT_NAME}'..."
        )

    add_custom_target(${TARGET} DEPENDS ${OUTPUT_DLL})
endfunction()
