list(APPEND SANDBOX_SOURCES
    gui_layer.cpp
    sandbox.cpp
    triangle_layer.cpp
    drawable/cube.cpp
    drawable/drawable.cpp
    drawable/model.cpp
    drawable/shape.cpp
    drawable/triangle.cpp
    gui_layer/camera.cpp
    gui_layer/gui_layer_window.cpp
    )

list(APPEND SANDBOX_HEADERS
    gui_layer.h
    triangle_layer.h
    drawable/cube.h
    drawable/drawable.h
    drawable/model.h
    drawable/shape.h
    drawable/triangle.h
    gui_layer/camera.h
    gui_layer/gui_layer_window.h
    )

ge_add_executable(sandbox
    SOURCES ${SANDBOX_SOURCES} ${SANDBOX_HEADERS}
    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE_DEPS
        docopt_s
        genesis::genesis
        imgui
    )
