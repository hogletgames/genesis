list(APPEND LEVEL_EDITOR_SOURCES
    main.cpp
    gui/level_editor_gui.cpp
    gui/main_menu/assets_menu.cpp
    gui/main_menu/menu_bar.cpp
    gui/main_menu/project_menu.cpp
    gui/main_menu/scene_menu.cpp
    gui/main_menu/view_menu.cpp
    gui/modals/notification_modal.cpp
    gui/panels/assets_panel.cpp
    gui/panels/components_panel.cpp
    gui/panels/editor_camera_panel.cpp
    gui/panels/log_panel.cpp
    gui/panels/scene_panel.cpp
    gui/panels/viewport_panel.cpp
    gui/windows/add_mesh_resource_window.cpp
    gui/windows/add_pipeline_resources_window.cpp
    gui/windows/add_texture_resource_window.cpp
    )

list(APPEND LEVEL_EDITOR_HEADERS
    gui/level_editor_gui.h
    gui/main_menu.h
    gui/modals.h
    gui/panels.h
    gui/windows.h
    gui/main_menu/assets_menu.h
    gui/main_menu/menu_bar.h
    gui/main_menu/project_menu.h
    gui/main_menu/scene_menu.h
    gui/main_menu/view_menu.h
    gui/modals/notification_modal.h
    gui/panels/assets_panel.h
    gui/panels/components_panel.h
    gui/panels/editor_camera_panel.h
    gui/panels/log_panel.h
    gui/panels/scene_panel.h
    gui/panels/viewport_panel.h
    gui/windows/add_mesh_resource_window.h
    gui/windows/add_pipeline_resources_window.h
    gui/windows/add_texture_resource_window.h
    )

ge_add_executable(level_editor
    SOURCES ${LEVEL_EDITOR_SOURCES} ${LEVEL_EDITOR_HEADERS}
    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE_DEPS
        Boost::circular_buffer
        docopt_s
        genesis::genesis
        nlohmann_json::nlohmann_json
    )
