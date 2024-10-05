# Find headers
find_path(GE_SPIRV_CROSS_INCLUDE_DIR spirv_cross/spirv_cross.hpp
    HINTS $ENV{VULKAN_SDK}
    PATH_SUFFIXES include
    )

# Find SPIRV-Cross core library
find_library(GE_SPIRV_CROSS_CORE_LIBRARY
    NAMES spirv-cross-core
    HINTS $ENV{VULKAN_SDK}
    PATH_SUFFIXES lib
    )

# Find SPIRV-Cross cpp library
find_library(GE_SPIRV_CROSS_CPP_LIBRARY
    NAMES spirv-cross-cpp
    HINTS $ENV{VULKAN_SDK}
    PATH_SUFFIXES lib
    )

# Verify variables
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GE_SPIRV_CROSS
    REQUIRED_VARS GE_SPIRV_CROSS_CORE_LIBRARY
                  GE_SPIRV_CROSS_CPP_LIBRARY
                  GE_SPIRV_CROSS_INCLUDE_DIR
    )

# Create imported targets
add_library(spirv-cross-core STATIC IMPORTED)
set_target_properties(spirv-cross-core PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${GE_SPIRV_CROSS_INCLUDE_DIR}
    IMPORTED_LOCATION             ${GE_SPIRV_CROSS_CORE_LIBRARY}
    )

add_library(spirv-cross-cpp STATIC IMPORTED)
set_target_properties(spirv-cross-cpp PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${GE_SPIRV_CROSS_INCLUDE_DIR}
    INTERFACE_LINK_LIBRARIES      spirv-cross-core
    IMPORTED_LOCATION             ${GE_SPIRV_CROSS_CPP_LIBRARY}
    )

