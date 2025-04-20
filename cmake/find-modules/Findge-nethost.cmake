if (NOT DEFINED ENV{DOTNET_ROOT})
    message(WARNING "Set DOTNET_ROOT environment variable to the .NET SDK path")
endif ()

find_path(NETHOST_INCLUDE
    NAMES nethost.h
    PATHS $ENV{DOTNET_ROOT}/packs/*/*/runtimes/*/native
    NO_DEFAULT_PATH
    REQUIRED
    )

find_library(NETHOST_LIBRARY
    NAMES nethost
    PATHS $ENV{DOTNET_ROOT}/packs/*/*/runtimes/*/native
    NO_DEFAULT_PATH
    REQUIRED
    )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ge-nethost
    REQUIRED_VARS NETHOST_LIBRARY NETHOST_INCLUDE
    )

add_library(nethost SHARED IMPORTED)
set_target_properties(nethost PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${NETHOST_INCLUDE}
    IMPORTED_LOCATION             ${NETHOST_LIBRARY}
    )

set(NETHOST_LIBRARIES ${NETHOST_LIBRARY} CACHE FILEPATH "nethost library" FORCE)
set(NETHOST_INCLUDE_DIRS ${NETHOST_INCLUDE} CACHE PATH "nethost include directory" FORCE)
