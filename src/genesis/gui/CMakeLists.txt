set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/gui)

list(APPEND GUI_SOURCES
    base_layer.cpp
    renderer.cpp
    widgets/button.cpp
    widgets/checkbox.cpp
    widgets/combo_box.cpp
    widgets/image.cpp
    widgets/input_text.cpp
    widgets/main_menu_bar.cpp
    widgets/menu.cpp
    widgets/menu_item.cpp
    widgets/same_line.cpp
    widgets/separator.cpp
    widgets/style_var.cpp
    widgets/text.cpp
    widgets/tree_node.cpp
    widgets/value_editor.cpp
    widgets/window.cpp
    )

list(APPEND GUI_HEADERS
    ${INCLUDE_DIR}/base_layer.h
    ${INCLUDE_DIR}/context.h
    ${INCLUDE_DIR}/event_handler.h
    ${INCLUDE_DIR}/renderer.h
    ${INCLUDE_DIR}/widgets.h
    ${INCLUDE_DIR}/widgets/button.h
    ${INCLUDE_DIR}/widgets/checkbox.h
    ${INCLUDE_DIR}/widgets/combo_box.h
    ${INCLUDE_DIR}/widgets/image.h
    ${INCLUDE_DIR}/widgets/input_text.h
    ${INCLUDE_DIR}/widgets/main_menu_bar.h
    ${INCLUDE_DIR}/widgets/menu.h
    ${INCLUDE_DIR}/widgets/menu_item.h
    ${INCLUDE_DIR}/widgets/same_line.h
    ${INCLUDE_DIR}/widgets/separator.h
    ${INCLUDE_DIR}/widgets/style_var.h
    ${INCLUDE_DIR}/widgets/text.h
    ${INCLUDE_DIR}/widgets/tree_node.h
    ${INCLUDE_DIR}/widgets/value_editor.h
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
    PUBLIC_DEPS
        Boost::signals2
        genesis::app
    PRIVATE_DEPS
        genesis::graphics
        imgui
    )
