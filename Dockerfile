FROM ubuntu:bionic

# Build tools
RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y software-properties-common make cmake wget

# git
# actions/checkout@v2 requires git v2.18 and later for submodules
RUN add-apt-repository ppa:git-core/ppa && apt-get update && \
    apt-get -y install git

# SDL2 dependencies
RUN apt-get install -y libx11-dev libsamplerate-dev libasound2-dev \
                       libjack-dev libpulse-dev libsndio-dev \
                       libxcursor-dev libxinerama-dev libxi-dev \
                       libxrandr-dev libxss-dev libxxf86vm-dev \
                       libdbus-1-dev

# Vulkan dependencies
RUN apt-get install -y libwayland-dev

# Compilers
ENV GCC_VERSION=11
RUN apt-add-repository ppa:ubuntu-toolchain-r/test && \
    apt-get update && apt-get install -y gcc-${GCC_VERSION} g++-${GCC_VERSION}

# Clang tools
ENV CLANG_VERSION=13
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - && \
    apt-add-repository "deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-${CLANG_VERSION} main" && \
    apt-get update && apt-get install -y clang-format-${CLANG_VERSION} clang-tidy-${CLANG_VERSION}

# Update alternatives
COPY tools/update_alternatives_gcc.sh tools/update_alternatives_clang.sh /tmp/
RUN /tmp/update_alternatives_gcc.sh "${GCC_VERSION}" "${GCC_VERSION}0" && \
    /tmp/update_alternatives_clang.sh "${CLANG_VERSION}" "${CLANG_VERSION}0"
