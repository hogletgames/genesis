set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/gui)

list(APPEND GUI_SOURCES
    base_layer.cpp
    renderer.cpp
    widgets/image.cpp
    widgets/window.cpp
    )

list(APPEND GUI_HEADERS
    ${INCLUDE_DIR}/base_layer.h
    ${INCLUDE_DIR}/context.h
    ${INCLUDE_DIR}/event_handler.h
    ${INCLUDE_DIR}/renderer.h
    ${INCLUDE_DIR}/widgets.h
    ${INCLUDE_DIR}/widgets/image.h
    ${INCLUDE_DIR}/widgets/widget_node.h
    ${INCLUDE_DIR}/widgets/widget_node_guard.h
    ${INCLUDE_DIR}/widgets/window.h
    private/types.h
    )

ge_add_module(gui
    SOURCES ${GUI_SOURCES} ${GUI_HEADERS}
    INCLUDE_DIRS
        ${INCLUDE_DIR}
        ${INCLUDE_DIR}/widgets
        ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC_DEPS genesis::app
    PRIVATE_DEPS
        genesis::graphics
        imgui
    )
