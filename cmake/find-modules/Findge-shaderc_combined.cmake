# Find headers
find_path(GE_SHADERC_INCLUDE_DIR shaderc/shaderc.hpp
    HINTS $ENV{VULKAN_SDK}
    PATH_SUFFIXES include
)

# Find the library
find_library(GE_SHADERC_COMBINED_LIBRARY
    NAMES shaderc_combined
    HINTS $ENV{VULKAN_SDK}
    PATH_SUFFIXES lib
)

# Verify variables
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GE_SHADERC_COMBINED
    REQUIRED_VARS GE_SHADERC_COMBINED_LIBRARY
                  GE_SHADERC_INCLUDE_DIR
    )

# Create imported targets
add_library(shaderc_combined STATIC IMPORTED)
set_target_properties(shaderc_combined PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${GE_SHADERC_INCLUDE_DIR}
    IMPORTED_LOCATION             ${GE_SHADERC_COMBINED_LIBRARY}
    )
