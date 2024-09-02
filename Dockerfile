# CMake build image
FROM ubuntu:focal AS cmake-builder

# Install build tools
RUN --mount=type=cache,target=/var/cache/apt \
    apt-get update && apt-get install -y --no-install-recommends \
        build-essential ca-certificates git libssl-dev

# Build and install CMake
RUN git clone --depth 1 --branch v3.17.4 https://github.com/Kitware/CMake /tmp/cmake \
    && mkdir -p /tmp/cmake/build && cd /tmp/cmake/build \
    && ../bootstrap --parallel=$(nproc) --prefix=/opt/cmake \
    && make -j$(nproc) && make install \
    && rm -rf /tmp/cmake

# Genesis image
FROM ubuntu:focal AS genesis-image

# Arguments
ARG GCC_VER=11   \
    CLANG_VER=16

ENV DEBIAN_FRONTEND="noninteractive"

# Install cmake
COPY --from=cmake-builder /opt/cmake/ /usr/local/

# Instal essential packages
RUN --mount=type=cache,target=/var/cache/apt \
    apt-get update && apt-get install -y --no-install-recommends \
        gpg-agent software-properties-common wget \
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
    && git config --add --system user.email "hogletgames@gmail.com"

# SDL2 dependencies
RUN --mount=type=cache,target=/var/cache/apt \
    apt-get update && apt-get install -y --no-install-recommends \
        libx11-dev libsamplerate-dev libasound2-dev libjack-dev libpulse-dev libsndio-dev \
        libxcursor-dev libxinerama-dev libxi-dev libxrandr-dev libxss-dev libxxf86vm-dev \
        libdbus-1-dev

# Vulkan dependencies
RUN --mount=type=cache,target=/var/cache/apt \
    apt-get update && apt-get install -y --no-install-recommends \
        libwayland-dev

ENV CC="gcc-${GCC_VER}" \
    CXX="g++-${GCC_VER}" \
    CLANG_FORMAT_BIN="clang-format-${CLANG_VER}" \
    RUN_CLANG_TIDY_BIN="run-clang-tidy-${CLANG_VER}"
