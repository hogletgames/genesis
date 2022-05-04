set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/gui)

list(APPEND GUI_SOURCES
    base_layer.cpp
    renderer.cpp)

list(APPEND GUI_HEADERS
    ${INCLUDE_DIR}/base_layer.h
    ${INCLUDE_DIR}/context.h
    ${INCLUDE_DIR}/renderer.h)

ge_add_module(gui
    SOURCES ${GUI_SOURCES} ${GUI_HEADERS}
    INCLUDE_DIR ${INCLUDE_DIR}
    PUBLIC_DEPS genesis::app
    PRIVATE_DEPS
        genesis::graphics
        imgui::imgui)
