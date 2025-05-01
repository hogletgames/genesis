include(GoogleTest)

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
