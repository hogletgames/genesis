list(APPEND TEST_SOURCES
    mesh_resource_test.cpp
    resource_id_test.cpp
    )

list(APPEND TEST_HEADERS
    common/resource_visitor_mock.h
    )

add_executable(genesis_assets_test ${TEST_SOURCES} ${TEST_HEADERS})
target_link_libraries(genesis_assets_test PRIVATE
    genesis::assets
    gmock
    gtest_main
    )

add_test(NAME genesis_assets_test COMMAND genesis_core_test)
