if (NOT DEFINED ENV{DOTNET_ROOT})
    message(WARNING "Set DOTNET_ROOT environment variable to the .NET SDK path")
endif ()

find_path(HOSTFXR_INCLUDE
    NAMES hostfxr.h
    PATHS $ENV{DOTNET_ROOT}/packs/*/*/runtimes/*/native
    NO_DEFAULT_PATH
    REQUIRED
    )

find_library(HOSTFXR_LIBRARY
    NAMES hostfxr
    PATHS $ENV{DOTNET_ROOT}/host/fxr/*
    NO_DEFAULT_PATH
    REQUIRED
    )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ge-hostfxr
    REQUIRED_VARS HOSTFXR_LIBRARY HOSTFXR_INCLUDE
    )

add_library(hostfxr SHARED IMPORTED)
set_target_properties(hostfxr PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${HOSTFXR_INCLUDE}
    IMPORTED_LOCATION             ${HOSTFXR_LIBRARY}
    )

set(HOSTFXR_LIBRARIES ${HOSTFXR_LIBRARY} CACHE FILEPATH "hostfxr library" FORCE)
set(HOSTFXR_INCLUDE_DIRS ${HOSTFXR_INCLUDE} CACHE PATH "hostfxr include directory" FORCE)
