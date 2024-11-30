## genesis

[![CI](https://github.com/hogletgames/genesis/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/hogletgames/genesis/actions/workflows/ci.yml)

### Acknowledgements

- [SDL](https://github.com/libsdl-org/SDL.git) ([zlib](https://github.com/libsdl-org/SDL/blob/main/LICENSE.txt))
- [spdlog](https://github.com/gabime/spdlog) ([MIT](https://github.com/gabime/spdlog/blob/v1.x/README.md))
- [glm](https://github.com/g-truc/glm) ([MIT](https://github.com/g-truc/glm/blob/master/copying.txt))
- [Vulkan SDK](https://vulkan.lunarg.com/#new_tab)
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
- [box2d](https://github.com/erincatto/box2d) ([MIT](https://github.com/erincatto/box2d/blob/main/LICENSE))

### Dependencies

You need to install the following dependencies to build the project:

- [CMake](https://cmake.org/download/) v3.22 or higher
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home) v1.3.268 or higher
- [Boost](https://www.boost.org/) v1.82 or higher

### To build manually

```bash
make -j$(nproc)
```

### To build using Docker

```bash
make docker_initialize # Create Docker images
make docker_build      # Build the project inside the build-env container
```

### Using the runtime-env docker-compose service

```bash
make docker_initialize # Create Docker images 
make docker_env_up     # Run the runtime-env service in the background
make docker_env_attach # Attach to the runtime-env container
# Run any command inside the runtime-env service
make docker_env_down    # Stop the runtime-env container
```

You can use `CLANG_FORMAT_BIN` and `RUN_CLANG_TIDY_BIN` `make` options to pass
path to an appropriate binary file.

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

*Notes:*

- `VK_LAYER_PATH` should point to the directory where
  `VK_LAYER_LUNARG_standard_validation.json` is located
- For MacOS `VK_ICD_FILENAMES` should point to the directory where
  `MoltenVK_icd.json`

#### Linux

```bash
export VK_ADD_LAYER_PATH=${VULKAN_SDK}/share/vulkan/explicit_layer.d
build/examples/sandbox/sandbox -e gui
```

#### MacOS

```bash
export VK_ADD_LAYER_PATH=${VULKAN_SDK}/share/vulkan/explicit_layer.d
export VK_ICD_FILENAMES=${VULKAN_SDK}/share/vulkan/icd.d/MoltenVK_icd.json
build/examples/sandbox/sandbox -e gui
```

### genesis build options

| Make                 | CMake                | Default value     | Description                                                                        |
|----------------------|----------------------|-------------------|------------------------------------------------------------------------------------|
| `BUILD_TYPE`         | `CMAKE_BUILD_TYPE`   | `Release`         | Project build types: `Release`, `Debug`, `RelWithDebInfo`, `ASAN`, `UBSAN`, `TSAN` |
| `BUILD_STATIC`       | `GE_STATIC`          | `OFF`             | Build static library                                                               |
| `DISABLE_ASSERTS`    | `GE_DISABLE_ASSERTS` | `OFF`             | Exclude asserts from final binary                                                  |
| `BUILD_APPS`         | `GE_BUILD_APPS`      | `OFF`             | Build applications                                                                 |
| `BUILD_EXAMPLES`     | `GE_BUILD_EXAMPLES`  | `OFF`             | Build examples                                                                     |
| `BUILD_TESTS`        | `GE_BUILD_TESTS`     | `OFF`             | Build tests                                                                        |
| `CLANG_FORMAT_BIN`   | -                    | `clang-format`    | Path to `clang-format` binary                                                      |
| `RUN_CLANG_TIDY_BIN` | -                    | `run-clang-tidy`  | Path to `run-clang-tidy` tool                                                      |

### Licence

**genesis** is licensed under the [BSD 3-Clause license](LICENSE).
