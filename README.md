## genesis

### Acknowledgements

- [SDL](https://www.libsdl.org) ([zlib](https://www.libsdl.org/license.php))
- [spdlog](https://github.com/gabime/spdlog) ([MIT](https://github.com/gabime/spdlog/blob/v1.8.2/LICENSE))
- [glm](https://github.com/g-truc/glm) ([MIT](https://github.com/g-truc/glm/blob/0.9.9.8/copying.txt))
- Vulkan SDK:
  - [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers) ([Apache 2.0](https://github.com/KhronosGroup/Vulkan-Headers/blob/sdk-1.2.162.0/LICENSE.txt))

### To build manually

```bash
make -j$(nproc)
```

### To build using Docker

```bash
make docker_initialize  # Create Docker image
make docker_build       # Build project inside Docker container
make docker_cleenup     # Remove docker container (optional)
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

### genesis build options

| Make | CMake | Default value| Description |
|------|-------|--------------|-------------|
| `BUILD_TYPE` | `CMAKE_BUILD_TYPE` | `Release` | Project build types: `Release`, `Debug`, `ASAN`, `USAN`, `TSAN` |
| `BUILD_STATIC` | `GE_STATIC` | `OFF` | Build static library |
| `DISABLE_ASSERTS` | `GE_DISABLE_ASSERTS` | `OFF` | Exclude asserts from final binary |
| `BUILD_EXAMPLES` | `GE_BUILD_EXAMPLES` | `OFF` | Build examples |
| `CLANG_FORMAT_BIN` | - | `clang-format` | Path to `clang-format` binary |
| `RUN_CLANG_TIDY_BIN` | - | `run-clang-tidy` | Path to `run-clang-tidy` tool |
| `DOCKER_CMD` | - | `make -j$(nproc)` | Command which will be executed by `make docker_run` |

### Licence

**genesis** is licensed under the [BSD 3-Clause license](LICENSE).
