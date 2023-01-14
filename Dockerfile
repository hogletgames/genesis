FROM ubuntu:focal

# Arguments
ARG DEBIAN_FRONTEND="noninteractive" \
    GCC_VER=11 \
    CLANG_VER=13

RUN apt-get update && apt-get install -y --no-install-recommends \
    gpg-agent software-properties-common wget && \
# GCC-11
    apt-add-repository ppa:ubuntu-toolchain-r/test && \
# Clang-13
    wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
    apt-add-repository "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-${CLANG_VER} main" && \
# Essential build tools
    apt-get update && apt-get install -y --no-install-recommends  \
    gcc-${GCC_VER} g++-${GCC_VER} clang-format-${CLANG_VER} clang-tidy-${CLANG_VER}  \
    make cmake git patch && \
# Clean-up atp cache
    rm -rf /var/lib/apt/lists/* && \
# Configure git
    git config --add --system user.name "hogletgames" && \
    git config --add --system user.email "hogletgames@gmail.com"

# SDL2 dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    libx11-dev libsamplerate-dev libasound2-dev libjack-dev libpulse-dev libsndio-dev \
    libxcursor-dev libxinerama-dev libxi-dev libxrandr-dev libxss-dev libxxf86vm-dev \
    libdbus-1-dev && \
# Clean-up atp cache
    rm -rf /var/lib/apt/lists/*

# Vulkan dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    libwayland-dev && \
# Clean-up atp cache
    rm -rf /var/lib/apt/lists/*

ENV CC="gcc-${GCC_VER}" \
    CXX="g++-${GCC_VER}" \
    CLANG_FORMAT_BIN="clang-format-${CLANG_VER}" \
    RUN_CLANG_TIDY_BIN="run-clang-tidy-${CLANG_VER}"
