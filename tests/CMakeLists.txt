include(${PROJECT_SOURCE_DIR}/cmake/AddGTest.cmake)

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

add_subdirectory(core)
add_subdirectory(graphics)
add_subdirectory(scene)
add_subdirectory(window)

if (NOT BUILD_TYPE STREQUAL "ASAN")
    message(STATUS "Enabling genesis::script tests for ${BUILD_TYPE} build type")
    add_subdirectory(script)
else ()
    message(STATUS "Disabling genesis::script tests for ${BUILD_TYPE} build type")
endif ()
