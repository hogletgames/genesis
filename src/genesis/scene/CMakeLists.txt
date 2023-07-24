set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/scene)

list(APPEND SCENE_HEADERS
    ${INCLUDE_DIR}/component_list.h
    ${INCLUDE_DIR}/components.h
    ${INCLUDE_DIR}/entity.h
    ${INCLUDE_DIR}/registry.h
    ${INCLUDE_DIR}/scene.h
    ${INCLUDE_DIR}/camera/projection_camera.h
    ${INCLUDE_DIR}/camera/view_projection_camera.h
    ${INCLUDE_DIR}/camera/vp_camera_controller.h
    ${INCLUDE_DIR}/components/camera_component.h
    ${INCLUDE_DIR}/components/material_component.h
    ${INCLUDE_DIR}/components/sprite_component.h
    ${INCLUDE_DIR}/components/tag_component.h
    ${INCLUDE_DIR}/components/transform_component.h
    ${INCLUDE_DIR}/components/yaml_convert.h
    )

list(APPEND SCENE_SOURCES
    entity.cpp
    registry.cpp
    scene.cpp
    camera/projection_camera.cpp
    camera/view_projection_camera.cpp
    camera/vp_camera_controller.cpp
    )

ge_add_module(scene
    SOURCES ${SCENE_SOURCES} ${SCENE_HEADERS}
    INCLUDE_DIRS ${INCLUDE_DIR}
    PUBLIC_DEPS
        EnTT::EnTT
        genesis::assets
        genesis::core
        genesis::graphics
        genesis::math
        yaml-cpp::yaml-cpp
    PRIVATE_DEPS
        genesis::window
    )
