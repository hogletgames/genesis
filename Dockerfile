# Vulkan SDK builder
FROM ubuntu:jammy AS vulkan-sdk-builder
ENV DEBIAN_FRONTEND="noninteractive"

# Install required packages
RUN --mount=type=cache,sharing=locked,target=/var/cache/apt                    \
    apt-get update && apt-get install -y --no-install-recommends               \
# Essential build tools
        build-essential                                                        \
        ca-certificates                                                        \
        cmake                                                                  \
        git                                                                    \
        pkg-config                                                             \
        python3                                                                \
# Vulkan SDK dependencies
        libx11-dev                                                             \
        libxrandr-dev                                                          \
        libwayland-dev                                                         \
        xorg-dev

# Build and install Vulkan SDK
ARG VULKAN_SDK_VER="1.3.268.0"
COPY tools/install_vulkan_sdk_linux.sh /tmp/tools/install_vulkan_sdk_linux.sh
RUN bash /tmp/tools/install_vulkan_sdk_linux.sh                                \
        "${VULKAN_SDK_VER}"                                                    \
        "/opt/vulkan-sdk"

# Boost library builder
FROM ubuntu:jammy AS boost-builder

# Install essential build tools
RUN --mount=type=cache,sharing=locked,target=/var/cache/apt                    \
    apt-get update && apt-get install -y --no-install-recommends               \
        build-essential                                                        \
        ca-certificates                                                        \
        wget

# Build and install Boost
COPY tools/install_boost_linux.sh /tmp/tools/install_boost_linux.sh
RUN bash /tmp/tools/install_boost_linux.sh "/opt/boost"

# Genesis image
FROM ubuntu:jammy AS genesis-image
ARG GCC_VER=11                                                                 \
    CLANG_VER=20
ENV DEBIAN_FRONTEND="noninteractive"

# Install required packages from previous stages
COPY --from=vulkan-sdk-builder /opt/vulkan-sdk /opt/vulkan-sdk
COPY --from=boost-builder      /opt/boost      /opt/boost

# Instal essential packages
RUN --mount=type=cache,sharing=locked,target=/var/cache/apt                    \
    apt-get update && apt-get install -y --no-install-recommends               \
        gpg-agent                                                              \
        software-properties-common                                             \
        wget                                                                   \
# GCC
 && apt-add-repository ppa:ubuntu-toolchain-r/test                             \
# Clang tools
 && wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -       \
 && apt-add-repository                                                         \
        "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-${CLANG_VER} main"\
# Essential build tools
 && apt-get update && apt-get install -y --no-install-recommends               \
        build-essential                                                        \
        gcc-${GCC_VER}                                                         \
        g++-${GCC_VER}                                                         \
        clang-format-${CLANG_VER}                                              \
        clang-tidy-${CLANG_VER}                                                \
        cmake                                                                  \
        git                                                                    \
        patch                                                                  \
        libgtk-3-dev                                                           \
# Development tools
        bash-completion                                                        \
        gdb                                                                    \
        valgrind                                                               \
        vim                                                                    \
# Configure git
 && git config --add --system user.name "hogletgames"                          \
 && git config --add --system user.email "hogletgames@gmail.com"               \
 && git config --add --system safe.directory "*"

# SDL2 dependencies
RUN --mount=type=cache,sharing=locked,target=/var/cache/apt                    \
    apt-get update && apt-get install -y --no-install-recommends               \
        libx11-dev                                                             \
        libsamplerate-dev                                                      \
        libasound2-dev                                                         \
        libjack-dev                                                            \
        libpulse-dev                                                           \
        libsndio-dev                                                           \
        libxcursor-dev                                                         \
        libxinerama-dev                                                        \
        libxi-dev                                                              \
        libxrandr-dev                                                          \
        libxss-dev                                                             \
        libxxf86vm-dev                                                         \
        libdbus-1-dev

# Environment
ENV CC="gcc-${GCC_VER}"                                                        \
    CXX="g++-${GCC_VER}"                                                       \
    CLANG_FORMAT_BIN="clang-format-${CLANG_VER}"                               \
    RUN_CLANG_TIDY_BIN="run-clang-tidy-${CLANG_VER}"                           \
    VULKAN_SDK="/opt/vulkan-sdk"                                               \
    BOOST_ROOT="/opt/boost"                                                    \
