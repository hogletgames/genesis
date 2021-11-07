set(GE_THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/third-party)
set(GE_ROOT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
list(APPEND DEBUG_BUILD_LIST Debug RelWithDebInfo)

function(ge_add_compile_options TARGET)
    list(APPEND GE_GNU_COMPILE_FLAGS -Werror -Wextra -Wall -Wpedantic -Wshadow)

    target_compile_options(${TARGET} PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:${GE_GNU_COMPILE_FLAGS}>
    )
endfunction()

function(ge_default_target_config TARGET)
    ge_add_compile_options(${TARGET})
    target_include_directories(${TARGET} PUBLIC ${GE_ROOT_INCLUDE_DIR})
endfunction()
