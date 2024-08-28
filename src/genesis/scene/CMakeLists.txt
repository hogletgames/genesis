set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis/scene)

list(APPEND SCENE_HEADERS
    ${INCLUDE_DIR}/component_list.h
    ${INCLUDE_DIR}/components.h
    ${INCLUDE_DIR}/entity.h
    ${INCLUDE_DIR}/entity_factory.h
    ${INCLUDE_DIR}/entity_node.h
    ${INCLUDE_DIR}/entity_picker.h
    ${INCLUDE_DIR}/pipeline_library.h
    ${INCLUDE_DIR}/registry.h
    ${INCLUDE_DIR}/renderer.h
    ${INCLUDE_DIR}/scene.h
    ${INCLUDE_DIR}/scene_deserializer.h
    ${INCLUDE_DIR}/scene_serializer.h
    ${INCLUDE_DIR}/camera/projection_camera.h
    ${INCLUDE_DIR}/camera/view_projection_camera.h
    ${INCLUDE_DIR}/camera/vp_camera_controller.h
    ${INCLUDE_DIR}/components/camera_component.h
    ${INCLUDE_DIR}/components/material_component.h
    ${INCLUDE_DIR}/components/relationship_components.h
    ${INCLUDE_DIR}/components/sprite_component.h
    ${INCLUDE_DIR}/components/tag_component.h
    ${INCLUDE_DIR}/components/transform_component.h
    ${INCLUDE_DIR}/components/yaml_convert.h
    ${INCLUDE_DIR}/renderer/irenderer.h
    ${INCLUDE_DIR}/renderer/plain_renderer.h
    ${INCLUDE_DIR}/renderer/renderer_base.h
    ${INCLUDE_DIR}/renderer/wb_oit_renderer.h
    )

list(APPEND SCENE_SOURCES
    entity.cpp
    entity_factory.cpp
    entity_node.cpp
    entity_picker.cpp
    registry.cpp
    scene.cpp
    scene_deserializer.cpp
    scene_serializer.cpp
    camera/projection_camera.cpp
    camera/view_projection_camera.cpp
    camera/vp_camera_controller.cpp
    renderer/plain_renderer.cpp
    renderer/renderer_base.cpp
    renderer/wb_oit_renderer.cpp
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
        genesis::graphics
        genesis::window
    )
