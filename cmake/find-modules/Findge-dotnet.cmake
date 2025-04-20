if (NOT DEFINED ENV{DOTNET_ROOT})
    message(WARNING "Set DOTNET_ROOT environment variable to the .NET SDK path")
endif ()

find_file(DOTNET_EXE
    NAMES dotnet
    PATHS $ENV{DOTNET_ROOT}
    NO_DEFAULT_PATH
    )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ge-dotnet
    REQUIRED_VARS DOTNET_EXE
    )

add_executable(dotnet IMPORTED)
set_target_properties(dotnet PROPERTIES
    IMPORTED_LOCATION ${DOTNET_EXE}
    )

set(DOTNET_EXE ${DOTNET_EXE} CACHE FILEPATH ".NET executable" FORCE)
