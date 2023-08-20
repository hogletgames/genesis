FROM ubuntu:focal AS genesis-image

# Arguments
ARG DEBIAN_FRONTEND="noninteractive" \
    GCC_VER=11 \
    CLANG_VER=17

RUN apt-get update && apt-get install -y --no-install-recommends \
    gpg-agent software-properties-common wget && \
# GCC
    apt-add-repository ppa:ubuntu-toolchain-r/test && \
# Clang-tols
    wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
    apt-add-repository "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-${CLANG_VER} main" && \
# Essential build tools
    apt-get update && apt-get install -y --no-install-recommends  \
    build-essential gcc-${GCC_VER} g++-${GCC_VER} clang-format-${CLANG_VER} clang-tidy-${CLANG_VER} \
    cmake git patch && \
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

ENV CLANG_FORMAT_BIN="clang-format-${CLANG_VER}" \
    RUN_CLANG_TIDY_BIN="run-clang-tidy-${CLANG_VER}"
