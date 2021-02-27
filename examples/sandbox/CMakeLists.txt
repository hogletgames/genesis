add_executable(sandbox sandbox.cpp)
target_link_libraries(sandbox PRIVATE genesis::genesis)
ge_default_target_config(sandbox)
