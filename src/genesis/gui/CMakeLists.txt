set(GE_GUI_INCLUDE_DIR ${GE_INCLUDE_DIR}/gui)

list(APPEND GE_GUI_SRC
    renderer.cpp)

list(APPEND GE_GUI_HEADERS
    ${GE_GUI_INCLUDE_DIR}/context.h
    ${GE_GUI_INCLUDE_DIR}/renderer.h)


add_library(genesis-gui STATIC ${GE_GUI_SRC} ${GE_GUI_HEADERS})
target_link_libraries(genesis-gui
    PRIVATE
        genesis::renderer
        imgui)
target_include_directories(genesis-gui PRIVATE ${GE_GUI_INCLUDE_DIR})
ge_add_compile_options(genesis-gui)
add_library(genesis::gui ALIAS genesis-gui)
