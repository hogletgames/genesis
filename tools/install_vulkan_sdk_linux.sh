#!/usr/bin/env bash

set -e

function git_clone() {
  local URL="${1}"
  local SRC_DIR="${2}"
  local VERSION="${3}"

  git clone --depth 1 --branch "${VERSION}" "${URL}" "${SRC_DIR}"
}

function cmake_install() {
  local SRC_DIR="${1}"
  local INSTALL_PREFIX="${2}"
  local CMAKE_OPTIONS="${3}"

  local BUILD_DIR="${SRC_DIR}/build"

  cmake -S "${SRC_DIR}" -B "${BUILD_DIR}" \
    --install-prefix "${INSTALL_PREFIX}"  \
    -G "Unix Makefiles"                   \
    ${CMAKE_OPTIONS}
  cmake --build "${BUILD_DIR}" --parallel "$(nproc)" --config Release
  cmake --install "${BUILD_DIR}" --config Release
}

function install_package() {
  local NAME="${1}"
  local VERSION="${2}"
  local INSTALL_PREFIX="${3}"
  local CMAKE_OPTIONS="${4}"

  local SRC_DIR="/tmp/${NAME}"

  git_clone "https://github.com/hogletgames/${NAME}.git" "${SRC_DIR}" "${VERSION}"
  cmake_install "${SRC_DIR}" "${INSTALL_PREFIX}" "${CMAKE_OPTIONS}"
  rm -rf "${SRC_DIR}"
}

function install_spirv_tools() {
  local VERSION="${1}"
  local INSTALL_PREFIX="${2}"

  local SRC_DIR="/tmp/SPIRV-Tools"
  local BUILD_DIR="${SRC_DIR}/build"

  # Clone the project and its dependencies
  git_clone "https://github.com/hogletgames/SPIRV-Tools.git" "${SRC_DIR}" "${VERSION}"
  git_clone                                             \
    "https://github.com/hogletgames/SPIRV-Headers.git"  \
    "${SRC_DIR}/external/spirv-headers"                 \
    "${VERSION}"

  # Build and install the project
  cmake_install "${SRC_DIR}" "${INSTALL_PREFIX}" "-DSPIRV_SKIP_TESTS=ON"

  # Clean up
  rm -rf "${SRC_DIR}"
}

function install_shaderc() {
  local VERSION="${1}"
  local INSTALL_PREFIX="${2}"

  local SRC_DIR="/tmp/shaderc"
  local BUILD_DIR="${SRC_DIR}/build"

  git_clone "https://github.com/hogletgames/shaderc.git" "${SRC_DIR}" "${VERSION}"
  python3 "${SRC_DIR}/utils/git-sync-deps"
  cmake_install         \
    "${SRC_DIR}"        \
    "${INSTALL_PREFIX}" \
    "-DSHADERC_SKIP_TESTS=ON -DSHADERC_SKIP_EXAMPLES=ON"

  rm -rf "${SRC_DIR}"
}

function main() {
  USAGE="Usage: $0 <VULKAN_SDK_VERSION> <INSTALL_PREFIX>"

  VULKAN_SDK_VERSION="vulkan-sdk-${1}"
  INSTALL_PREFIX="${2}"

  if [[ -z "${VULKAN_SDK_VERSION}" ]] || [[ -z "${INSTALL_PREFIX}" ]]; then
    echo "${USAGE}"
    exit 1
  fi

  install_package "Vulkan-Headers" "${VULKAN_SDK_VERSION}" "${INSTALL_PREFIX}"
  install_package "Vulkan-Loader" "${VULKAN_SDK_VERSION}" "${INSTALL_PREFIX}"
  install_package "SPIRV-Headers" "${VULKAN_SDK_VERSION}" "${INSTALL_PREFIX}"
  install_spirv_tools "${VULKAN_SDK_VERSION}" "${INSTALL_PREFIX}"
  install_package "SPIRV-Cross" "${VULKAN_SDK_VERSION}" "${INSTALL_PREFIX}"
  install_package             \
    "Vulkan-ValidationLayers" \
    "${VULKAN_SDK_VERSION}"   \
    "${INSTALL_PREFIX}"       \
    "-DCMAKE_BUILD_TYPE=Release -DUPDATE_DEPS=ON"
  install_shaderc "v2023.7" "${INSTALL_PREFIX}"
}

main "${@}"
