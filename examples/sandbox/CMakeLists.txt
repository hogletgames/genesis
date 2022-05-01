list(APPEND SANDBOX_SOURCES
    gui_layer.cpp
    render_layer.cpp
    sandbox.cpp
    triangle_layer.cpp)

list(APPEND SANDBOX_HEADERS
    gui_layer.h
    render_layer.h
    triangle_layer.h)

ge_add_executable(sandbox
    SOURCES ${SANDBOX_SOURCES} ${SANDBOX_HEADERS}
    PRIVATE_DEPS
        genesis::genesis
        imgui::imgui)
