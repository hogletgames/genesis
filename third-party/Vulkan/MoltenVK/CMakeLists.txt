set(MOLTEN_VK_TARGET macos)
set(MOLTEN_VK_SRC ${CMAKE_CURRENT_SOURCE_DIR}/MoltenVK)

message(STATUS "MoltenVK target: ${MOLTEN_VK_TARGET}")

include(ExternalProject)
ExternalProject_Add(libMoltenVK
    SOURCE_DIR ${MOLTEN_VK_SRC}
    BUILD_IN_SOURCE true
    CONFIGURE_COMMAND ./fetchDependencies --${MOLTEN_VK_TARGET}
    BUILD_COMMAND make ${MOLTEN_VK_TARGET}
    INSTALL_COMMAND "")

add_dependencies(vulkan libMoltenVK)
