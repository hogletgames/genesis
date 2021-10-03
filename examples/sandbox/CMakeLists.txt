list(APPEND SANDBOX_SRC
    gui_layer.cpp
    render_layer.cpp
    sandbox.cpp
    triangle_layer.cpp)

list(APPEND SANDBOX_HEADERS
    gui_layer.h
    render_layer.h
    triangle_layer.h)

add_executable(sandbox ${SANDBOX_SRC} ${SANDBOX_HEADERS})
target_link_libraries(sandbox PRIVATE
    genesis::genesis
    imgui)
target_include_directories(sandbox SYSTEM PRIVATE ${GE_THIRD_PARTY_DIR}/imgui)
ge_default_target_config(sandbox)
