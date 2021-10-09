set(GE_GUI_INCLUDE_DIR ${GE_INCLUDE_DIR}/gui)

list(APPEND GE_GUI_HEADERS
    ${GE_GUI_INCLUDE_DIR}/context.h)

add_library(genesis-gui INTERFACE ${GE_GUI_SRC} ${GE_GUI_HEADERS})
add_library(genesis::gui ALIAS genesis-gui)
