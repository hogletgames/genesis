set(INCLUDE_DIR ${GE_INCLUDE_DIR}/genesis)

add_definitions(-DGE_EXPORTS)

list(APPEND GENESIS_HEADERS
    ${INCLUDE_DIR}/app.h
    ${INCLUDE_DIR}/assets.h
    ${INCLUDE_DIR}/core.h
    ${INCLUDE_DIR}/gui.h
    ${INCLUDE_DIR}/genesis.h
    ${INCLUDE_DIR}/graphics.h
    ${INCLUDE_DIR}/math.h
    ${INCLUDE_DIR}/scene.h
    ${INCLUDE_DIR}/window.h
    )

add_library(genesis ${GE_LIB_TYPE} ${GENESIS_HEADERS})
target_link_libraries(genesis PUBLIC
    genesis::app
    genesis::assets
    genesis::core
    genesis::graphics
    genesis::gui
    genesis::math
    genesis::scene
    genesis::window
    )
add_library(genesis::genesis ALIAS genesis)

add_subdirectory(app)
add_subdirectory(assets)
add_subdirectory(core)
add_subdirectory(graphics)
add_subdirectory(gui)
add_subdirectory(math)
add_subdirectory(scene)
add_subdirectory(window)
