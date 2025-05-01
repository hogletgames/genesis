add_subdirectory(genesis)

include(${PROJECT_SOURCE_DIR}/cmake/DotNetHelpers.cmake)
ge_dotnet_build(genesis-scripting/GenesisScripting.slnx)
