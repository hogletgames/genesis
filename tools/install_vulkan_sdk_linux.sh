#!/usr/bin/env bash

VULKAN_SDK_VERSION="1.3.268.0"
EXPECTED_SHA256="d23343736247828ff5b3b6b1b7fd83a72b5df1a54b2527ae3663cebdfee75842"

VULKAN_SDK_ARCHIVE_NAME="vulkansdk-linux-x86_64-${VULKAN_SDK_VERSION}.tar.xz"
VULKAN_SDK_URL="https://sdk.lunarg.com/sdk/download/${VULKAN_SDK_VERSION}/linux/${VULKAN_SDK_ARCHIVE_NAME}"
VULKAN_SDK_DOWNLOAD_DEST="/tmp/vulkan-sdk.tar.xz"
VULKAN_SDK_INSTALL_DEST="/opt/vulkan-sdk"

# Download the Vulkan SDK
wget -O "${VULKAN_SDK_DOWNLOAD_DEST}" "${VULKAN_SDK_URL}"

# Verify the SHA256 checksum
sha256 -c "${EXPECTED_SHA256}" "${VULKAN_SDK_DOWNLOAD_DEST}"

# Extract the Vulkan SDK
tar -xf "${VULKAN_SDK_DOWNLOAD_DEST}" -C /opt

# Append to the user's .bashrc
echo "source ${VULKAN_SDK_INSTALL_DEST}/${VULKAN_SDK_VERSION}/setup-env.sh" >> ~/.bashrc

# Cleanup
rm "${VULKAN_SDK_DOWNLOAD_DEST}"
