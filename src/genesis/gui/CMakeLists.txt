set(GE_GUI_INCLUDE_DIR ${GE_INCLUDE_DIR}/gui)

list(APPEND GE_GUI_SRC
    base_layer.cpp
    renderer.cpp)

list(APPEND GE_GUI_HEADERS
    ${GE_GUI_INCLUDE_DIR}/base_layer.h
    ${GE_GUI_INCLUDE_DIR}/context.h
    ${GE_GUI_INCLUDE_DIR}/renderer.h)

add_library(genesis-gui STATIC ${GE_GUI_SRC} ${GE_GUI_HEADERS})
target_link_libraries(genesis-gui
    PUBLIC
        genesis::app
    PRIVATE
        genesis::graphics
        imgui)
target_include_directories(genesis-gui PRIVATE ${GE_GUI_INCLUDE_DIR})
target_include_directories(genesis-gui SYSTEM PRIVATE ${GE_THIRD_PARTY_DIR}/imgui)
ge_add_compile_options(genesis-gui)
add_library(genesis::gui ALIAS genesis-gui)
