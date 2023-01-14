macro(ge_change_build_type TYPE)
    set(_GE_PREVIOUS_BUILD_TYPE ${CMAKE_BUILD_TYPE})
    set(CMAKE_BUILD_TYPE ${TYPE})
endmacro()

macro(ge_restore_build_type)
    set(CMAKE_BUILD_TYPE ${_GE_PREVIOUS_BUILD_TYPE})
    unset(_GE_PREVIOUS_BUILD_TYPE)
endmacro()
