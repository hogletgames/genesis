# CMake build image
FROM ubuntu:focal AS cmake-builder
ARG CMAKE_VER="v3.22.1"

RUN --mount=type=cache,sharing=locked,target=/var/cache/apt \
    apt-get update && apt-get install -y --no-install-recommends \
        build-essential ca-certificates git libssl-dev

RUN git clone --depth 1 --branch "${CMAKE_VER}" https://github.com/Kitware/CMake /tmp/cmake \
    && mkdir -p /tmp/cmake/build && cd /tmp/cmake/build \
    && ../bootstrap --parallel=$(nproc) --prefix=/opt/cmake \
    && make -j$(nproc) && make install \
    && rm -rf /tmp/cmake

# Vulkan SDK builder
FROM ubuntu:focal AS vulkan-sdk-builder
ARG VULKAN_SDK_VER="1.3.268.0"
ENV DEBIAN_FRONTEND="noninteractive"

COPY --from=cmake-builder /opt/cmake/ /usr/local/

RUN --mount=type=cache,sharing=locked,target=/var/cache/apt \
    apt-get update && apt-get install -y --no-install-recommends \
# Essential build tools
        build-essential ca-certificates git pkg-config python3 \
# Vulkan SDK dependencies
        libx11-dev libxrandr-dev libwayland-dev xorg-dev

COPY tools/install_vulkan_sdk_linux.sh /tmp/tools/install_vulkan_sdk_linux.sh
RUN bash /tmp/tools/install_vulkan_sdk_linux.sh "${VULKAN_SDK_VER}" "/opt/vulkan-sdk"

# Boost library builder
FROM ubuntu:focal AS boost-builder

RUN --mount=type=cache,sharing=locked,target=/var/cache/apt \
    apt-get update && apt-get install -y --no-install-recommends \
        build-essential ca-certificates wget

COPY tools/install_boost_linux.sh /tmp/tools/install_boost_linux.sh
RUN bash /tmp/tools/install_boost_linux.sh "/opt/boost"

# Mono library builder
FROM ubuntu:focal AS mono-builder
ARG MONO_VER="mono-6.12.0.206"
ENV DEBIAN_FRONTEND="noninteractive"

COPY --from=cmake-builder /opt/cmake/ /usr/local/

RUN --mount=type=cache,sharing=locked,target=/var/cache/apt \
    apt-get update && apt-get install -y --no-install-recommends \
# Essential build tools
        build-essential ca-certificates git python3 wget \
# Mono dependencies
        autoconf automake libtool gettext

COPY tools/install_mono_linux.sh /tmp/tools/install_mono_linux.sh
RUN bash /tmp/tools/install_mono_linux.sh "${MONO_VER}" "/opt/mono"

# Genesis image
FROM ubuntu:focal AS genesis-image

# Arguments
ARG GCC_VER=11 \
    CLANG_VER=19

ENV DEBIAN_FRONTEND="noninteractive"

# Install built tools
COPY --from=cmake-builder /opt/cmake/ /usr/local/
COPY --from=vulkan-sdk-builder /opt/vulkan-sdk /opt/vulkan-sdk
COPY --from=boost-builder /opt/boost /opt/boost
COPY --from=mono-builder /opt/mono /opt/mono

# Instal essential packages
RUN --mount=type=cache,sharing=locked,target=/var/cache/apt \
    apt-get update && apt-get install -y --no-install-recommends \
        gpg-agent software-properties-common wget vim \
# GCC
    && apt-add-repository ppa:ubuntu-toolchain-r/test \
# Clang tools
    && wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
    && apt-add-repository "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-${CLANG_VER} main" \
# Essential build tools
    && apt-get update && apt-get install -y --no-install-recommends  \
        build-essential gcc-${GCC_VER} g++-${GCC_VER} \
        clang-format-${CLANG_VER} clang-tidy-${CLANG_VER} \
        make git patch libgtk-3-dev \
# Configure git
    && git config --add --system user.name "hogletgames" \
    && git config --add --system user.email "hogletgames@gmail.com" \
    && git config --add --system safe.directory "*"

# SDL2 dependencies
RUN --mount=type=cache,sharing=locked,target=/var/cache/apt \
    apt-get update && apt-get install -y --no-install-recommends \
        libx11-dev libsamplerate-dev libasound2-dev libjack-dev libpulse-dev libsndio-dev \
        libxcursor-dev libxinerama-dev libxi-dev libxrandr-dev libxss-dev libxxf86vm-dev \
        libdbus-1-dev

# Environment
ENV CC="gcc-${GCC_VER}" \
    CXX="g++-${GCC_VER}" \
    CLANG_FORMAT_BIN="clang-format-${CLANG_VER}" \
    RUN_CLANG_TIDY_BIN="run-clang-tidy-${CLANG_VER}" \
    VULKAN_SDK="/opt/vulkan-sdk" \
    PKG_CONFIG_PATH="/opt/mono/lib/pkgconfig" \
    BOOST_ROOT="/opt/boost" \
    MONO_ROOT="/opt/mono"
