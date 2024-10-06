#!/usr/bin/env bash

set -e

MONO_VERSION="${1}"
INSTALL_PREFIX="${2}"

MONO_SRC_DIR="/tmp/mono"

# Validate input
if [[ -z "${MONO_VERSION}" ]] || [[ -z "${INSTALL_PREFIX}" ]]; then
  echo "Usage: $0 <MONO_VERSION> <INSTALL_PREFIX>"
  exit 1
fi

# Clone mono repository
git clone                                 \
  --branch "${MONO_VERSION}"              \
  --depth 1                               \
  https://github.com/hogletgames/mono.git \
  "${MONO_SRC_DIR}"

# Configure project
cd "${MONO_SRC_DIR}"
./autogen.sh --prefix="${INSTALL_PREFIX}"
make get-monolite-latest

# Build and install mono
make -j$(nproc)
make install

# Cleanup
rm -rf "${MONO_SRC_DIR}"
