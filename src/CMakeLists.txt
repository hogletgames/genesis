include(${PROJECT_SOURCE_DIR}/cmake/DotNetHelpers.cmake)

add_subdirectory(genesis)

ge_dotnet_build(genesis-scripting/GenesisScripting.sln)
