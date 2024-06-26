## genesis

[![CI](https://github.com/hogletgames/genesis/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/hogletgames/genesis/actions/workflows/ci.yml)

### Acknowledgements

- [SDL](https://github.com/libsdl-org/SDL.git) ([zlib](https://github.com/libsdl-org/SDL/blob/main/LICENSE.txt))
- [spdlog](https://github.com/gabime/spdlog) ([MIT](https://github.com/gabime/spdlog/blob/v1.x/README.md))
- [glm](https://github.com/g-truc/glm) ([MIT](https://github.com/g-truc/glm/blob/master/copying.txt))
- Vulkan SDK (sdk-1.3.236.0):
  - [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers) ([Apache 2.0](https://github.com/KhronosGroup/Vulkan-Headers/blob/main/LICENSE.txt))
  - [Vulkan-Loader](https://github.com/KhronosGroup/Vulkan-Loader) ([Apache 2.0](https://github.com/KhronosGroup/Vulkan-Loader/blob/master/LICENSE.txt))
  - [glslang](https://github.com/KhronosGroup/glslang) ([Multi-licensed](https://github.com/KhronosGroup/glslang/blob/master/LICENSE.txt))
  - [SPIRV-Headers](https://github.com/KhronosGroup/SPIRV-Headers) ([MIT](https://github.com/KhronosGroup/SPIRV-Headers/blob/main/LICENSE))
  - [SPIRV-Tools](https://github.com/KhronosGroup/SPIRV-Tools) ([Apache 2.0](https://github.com/KhronosGroup/SPIRV-Tools/blob/main/LICENSE))
  - [SPIRV-Cross](https://github.com/KhronosGroup/SPIRV-Cross) ([Apache 2.0](https://github.com/KhronosGroup/SPIRV-Cross/blob/main/LICENSE))
  - [Vulkan-ValidationLayers](https://github.com/KhronosGroup/Vulkan-ValidationLayers) ([Apache 2.0](https://github.com/KhronosGroup/Vulkan-ValidationLayers/blob/master/LICENSE.txt))
  - [shaderc](https://github.com/google/shaderc) ([Apache 2.0](https://github.com/google/shaderc/blob/main/LICENSE))
  - [MoltenVK](https://github.com/KhronosGroup/MoltenVK) ([Apache 2.0](https://github.com/KhronosGroup/MoltenVK/blob/main/LICENSE))
- [stb](https://github.com/nothings/stb) ([MIT](https://github.com/nothings/stb/blob/master/LICENSE))
- [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader) ([MIT](https://github.com/tinyobjloader/tinyobjloader/blob/release/CMakeLists.txt))
- [magic_enum](https://github.com/Neargye/magic_enum) ([MIT](https://github.com/Neargye/magic_enum/blob/master/LICENSE))
- [fmt](https://github.com/fmtlib/fmt) ([MIT](https://github.com/fmtlib/fmt/blob/master/LICENSE.rst))
- [googletest](https://github.com/google/googletest) ([BSD-3](https://github.com/google/googletest/blob/main/LICENSE))
- [imgui](https://github.com/ocornut/imgui) ([MIT](https://github.com/ocornut/imgui/blob/master/LICENSE.txt))
- [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo) ([MIT](https://github.com/CedricGuillemet/ImGuizmo/blob/master/LICENSE))
- [docopt.cpp](https://github.com/docopt/docopt.cpp) ([MIT](https://github.com/docopt/docopt.cpp/blob/master/LICENSE-MIT), [Boost 1.0](https://github.com/docopt/docopt.cpp/blob/master/LICENSE-Boost-1.0))
- [Boost](https://github.com/boostorg/boost/blob/master/CMakeLists.txt) ([BSL-1.0](https://github.com/boostorg/boost/blob/master/LICENSE_1_0.txt))
- [yaml-cpp](https://github.com/jbeder/yaml-cpp) ([MIT](https://github.com/jbeder/yaml-cpp/blob/master/LICENSE))
- [EnTT](https://github.com/skypjack/entt) ([MIT](https://github.com/skypjack/entt/blob/master/LICENSE))
- [nativefiledialog](https://github.com/mlabbe/nativefiledialog) ([zlib](https://github.com/mlabbe/nativefiledialog/blob/master/LICENSE))
- [nlohmann_json](https://github.com/nlohmann/json) ([MIT](https://github.com/nlohmann/json/blob/develop/LICENSE.MIT))

### To build manually

```bash
make -j$(nproc)
```

### To build using Docker

```bash
make docker_initialize  # Create Docker image
make docker_build       # Build project inside Docker container
make docker_cleanup     # Remove docker container (optional)
```

### To run custom command inside docker container

```bash
make DOCKER_CMD="make clang-tidy CLANG_TIDY_BIN=clang-tidy-11" docker_run
```

You can use `CLANG_FORMAT_BIN` and `RUN_CLANG_TIDY_BIN` `make` options to pass path to
appropriate binary file.

### Linters

```bash
make clang-foramt                   # Check format
make clang-tidy                     # Run clang-tidy check
bash tools/clang_format.sh --fix    # Fix format
```

### Hot to run tests

```bash
make BUILD_TESTS=ON -j$(nproc)  # Build project with tests
make test                       # Run tests
```

### To run examples

You don't need to install additional libraries to build or run examples or
applications. All you need to do is to set appropriate environment variables
depending on OS type.

At the moment `VK_LAYER_PATH` should only be configured for `Debug` or
`RelWithDebInfo` build types, to configure `Vulkan-ValidationLayers`.

#### Linux

```bash
export VK_LAYER_PATH=build/_deps/vulkan-validationlayers-build/layers
build/examples/sandbox/sandbox -e gui
```

#### MacOS

```bash
export VK_LAYER_PATH=build/_deps/vulkan-validationlayers-build/layers
export VK_ICD_FILENAMES=build/_deps/moltenvk-src/Package/Latest/MoltenVK/dynamic/dylib/macOS/MoltenVK_icd.json
build/examples/sandbox/sandbox -e gui
```

### genesis build options

| Make | CMake | Default value| Description |
|------|-------|--------------|-------------|
| `BUILD_TYPE` | `CMAKE_BUILD_TYPE` | `Release` | Project build types: `Release`, `Debug`, `ASAN`, `USAN`, `TSAN` |
| `BUILD_STATIC` | `GE_STATIC` | `OFF` | Build static library |
| `DISABLE_ASSERTS` | `GE_DISABLE_ASSERTS` | `OFF` | Exclude asserts from final binary |
| `BUILD_EXAMPLES` | `GE_BUILD_EXAMPLES` | `OFF` | Build examples |
| `BUILD_TESTS` | `GE_BUILD_TESTS` | `OFF` | Build tests |
| `CLANG_FORMAT_BIN` | - | `clang-format` | Path to `clang-format` binary |
| `RUN_CLANG_TIDY_BIN` | - | `run-clang-tidy` | Path to `run-clang-tidy` tool |
| `DOCKER_CMD` | - | `make -j$(nproc)` | Command which will be executed by `make docker_run` |

### Licence

**genesis** is licensed under the [BSD 3-Clause license](LICENSE).
