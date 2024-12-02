ge_add_executable(mono_scripting
    SOURCES mono_scripting.cpp
    PRIVATE_DEPS
        genesis::core
        PkgConfig::Mono
    )
target_compile_definitions(mono_scripting PRIVATE
    GE_EXAMPLES_DLL=\"${CMAKE_CURRENT_BINARY_DIR}/scripts/GeExamples.dll\"
    )

add_subdirectory(scripts)
add_dependencies(mono_scripting GeExamples.dll)
