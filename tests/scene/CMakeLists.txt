list(APPEND GE_SCENE_TEST_SRC
    scene_deserializer_test.cpp
    scene_serializer_test.cpp
    )

list(APPEND GE_SCENE_TEST_HEADERS
    component_matchers.h
    )

add_executable(genesis_scene_test ${GE_SCENE_TEST_SRC} ${GE_SCENE_TEST_HEADERS})
target_include_directories(genesis_scene_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(genesis_scene_test PRIVATE
    genesis::filesystem
    genesis::scene
    gmock
    gtest_main
    )

gtest_discover_tests(genesis_scene_test)
