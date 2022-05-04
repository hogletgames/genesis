macro(ge_ternary VAR CONDITION TRUE_VAL FALSE_VAL)
    if (${CONDITION})
        set(${VAR} ${TRUE_VAL})
    else ()
        set(${VAR} ${FALSE_VAL})
    endif ()
endmacro()

macro(ge_configure_debug_status)
    string(TOLOWER ${CMAKE_BUILD_TYPE} _GE_BUILD_TYPE)

    if (${_GE_BUILD_TYPE} MATCHES "(debug)|(relwithdebug)")
        set(GE_DISABLE_DEBUG OFF)
    else ()
        set(GE_DISABLE_DEBUG ON)
    endif ()

    unset(_GE_BUILD_TYPE)
endmacro()
