include(GoogleTest)

# ge_add_gtest(EXE_NAME
# [OPTIONS]
#   DO_NOT_LINK_MAIN <do not link gtest_main>
# [ONE_VALUE_KEYWORDS]
#   WORKING_DIRECTORY <working directory for tests>
# [MULTI_VALUE_KEYWORDS]
#   SOURCES <source files>
#   LIBS <libraries to link>
#   DEPS <dependencies to add>
#   DEFINITIONS <compile definitions>
# )
#
# This function adds a Google Test executable with the specified parameters.
# It allows you to specify sources, libraries, dependencies, compile
# definitions, whether to link the main gtest library, and the working directory
# for the tests. The function also automatically discovers tests in the
# executable and sets a timeout for them.
#
# Usage:
#   ge_add_gtest(my_test
#       SOURCES test1.cpp test2.cpp
#       LIBS mylib
#       DEPS my_dependency
#       DEFINITIONS MY_DEFINE=1
#       DO_NOT_LINK_MAIN
#       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests
#   )

function(ge_add_gtest EXE_NAME)
    cmake_parse_arguments(
        THIS
        "DO_NOT_LINK_MAIN"
        "WORKING_DIRECTORY"
        "SOURCES;LIBS;DEPS;DEFINITIONS"
        ${ARGN})

    if (THIS_DO_NOT_LINK_MAIN)
        set(GTEST_LIB gtest)
    else ()
        set(GTEST_LIB gtest_main)
    endif ()

    if (NOT THIS_WORKING_DIRECTORY)
        set(THIS_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    endif ()

    add_executable(${EXE_NAME} ${THIS_SOURCES})
    target_include_directories(${EXE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(${EXE_NAME} PRIVATE
        ${THIS_LIBS}
        ${GTEST_LIB}
        gmock
        )

    if (THIS_DEPS)
        add_dependencies(${EXE_NAME} ${THIS_DEPS})
    endif ()

    if (THIS_DEFINITIONS)
        target_compile_definitions(${EXE_NAME} PRIVATE ${THIS_DEFINITIONS})
    endif ()

    gtest_discover_tests(${EXE_NAME}
        WORKING_DIRECTORY ${THIS_WORKING_DIRECTORY}
        PROPERTIES TIMEOUT 60
        )
endfunction()
