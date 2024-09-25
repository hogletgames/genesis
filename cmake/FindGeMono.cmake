# Initialize variables
if (NOT DEFINED GeMono_USE_STATIC_LIB)
    set(GeMono_USE_STATIC_LIB FALSE)
endif ()

# Set library names based on the platform
if (WIN32)
    set(MONO_STATIC_LIB_NAME "mono-2.0.lib")
    set(MONO_SHARED_LIB_NAME "mono-2.0.dll")
elseif (APPLE)
    set(MONO_STATIC_LIB_NAME "libmono-2.0.a")
    set(MONO_SHARED_LIB_NAME "libmono-2.0.dylib")
else ()
    set(MONO_STATIC_LIB_NAME "libmono-2.0.a")
    set(MONO_SHARED_LIB_NAME "libmono-2.0.so")
endif ()

# Include directory
find_path(GeMono_INCLUDE_DIR
    NAMES jit.h
    PATH_SUFFIXES include/mono-2.0/mono/jit
    )

# Remove the last part of the path to refer to "include" directory
if (GeMono_INCLUDE_DIR)
    get_filename_component(GeMono_INCLUDE_DIR ${GeMono_INCLUDE_DIR} DIRECTORY)
    get_filename_component(GeMono_INCLUDE_DIR ${GeMono_INCLUDE_DIR} DIRECTORY)
    get_filename_component(GeMono_INCLUDE_DIR ${GeMono_INCLUDE_DIR} DIRECTORY)
endif ()

# Shared library
find_library(GeMono_SHARED_LIBRARIES
    NAMES ${MONO_SHARED_LIB_NAME}
    PATH_SUFFIXES lib
    )

# Static library
find_library(GeMono_STATIC_LIBRARIES
    NAMES ${MONO_STATIC_LIB_NAME}
    PATH_SUFFIXES lib
    )

# Handle the results
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GeMono
    REQUIRED_VARS GeMono_SHARED_LIBRARIES GeMono_STATIC_LIBRARIES GeMono_INCLUDE_DIR
    )

# Imported target for the shared library
add_library(ge-mono-shared SHARED IMPORTED)
set_target_properties(ge-mono-shared PROPERTIES
    IMPORTED_LOCATION ${GeMono_SHARED_LIBRARIES}
    INTERFACE_INCLUDE_DIRECTORIES ${GeMono_INCLUDE_DIR}
    )
add_library(ge-mono::mono-shared ALIAS ge-mono-shared)

# Imported target for the static library
add_library(ge-mono-static STATIC IMPORTED)
set_target_properties(ge-mono-static PROPERTIES
    IMPORTED_LOCATION ${GeMono_STATIC_LIBRARIES}
    INTERFACE_INCLUDE_DIRECTORIES ${GeMono_INCLUDE_DIR}
    )
add_library(ge-mono::mono-static ALIAS ge-mono-static)

# Set output variables
set(GeMono_FOUND TRUE CACHE BOOL "mono-2.0 has been found")
set(GeMono_INCLUDE_DIRS ${GeMono_INCLUDE_DIR} CACHE STRING "Local mono-2.0 include directories")

if (GeMono_USE_STATIC_LIB)
    set(GeMono_LIBRARIES ${GeMono_STATIC_LIBRARIES} CACHE STRING "Local mono-2.0 libraries")
else ()
    set(GeMono_LIBRARIES ${GeMono_SHARED_LIBRARIES} CACHE STRING "Local mono-2.0 libraries")
endif ()
