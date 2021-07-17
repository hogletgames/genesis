#!/usr/bin/env bash

set -e

VK_SDK_VERSION=${1}
VK_SDK_DEST="${2}"

USAGE="${0} <vk_sdk_version> <vk_sdk_dest>"

if [ -z "${VK_SDK_VERSION}" ] || [ -z "${VK_SDK_DEST}" ]; then
    echo "${USAGE}"
    exit 1
fi

VK_SDK_TAR="vulkansdk-linux-x86_64-${VK_SDK_VERSION}.tar.gz"
VK_SDK_URL="https://sdk.lunarg.com/sdk/download/${VK_SDK_VERSION}/linux/${VK_SDK_TAR}"

echo "Downloading ${VK_SDK_TAR}..."
wget "${VK_SDK_URL}" -P "${VK_SDK_DEST}" &> /dev/null

echo "Unzipping ${VK_SDK_TAR} to ${VK_SDK_DEST}..."
tar -xvf  "${VK_SDK_DEST}/${VK_SDK_TAR}" -C "${VK_SDK_DEST}" &> /dev/null

echo "Cleaning up..."
rm "${VK_SDK_DEST}/${VK_SDK_TAR}"
rm -r "${VK_SDK_DEST}/${VK_SDK_VERSION}/source"
