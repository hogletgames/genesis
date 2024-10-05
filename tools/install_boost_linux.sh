#!/usr/bin/env bash

set -e

INSTALL_PREFIX="${1:-/opt/boost}"

BOOST_URL=https://archives.boost.io/release/1.82.0/source/boost_1_82_0.tar.gz
BOOST_DEST_ARCHIVE=/tmp/boost.tar.gz
BOOST_SHA256="66a469b6e608a51f8347236f4912e27dc5c60c60d7d53ae9bfe4683316c6f04c"

# Download and extract the Boost library
wget -O "${BOOST_DEST_ARCHIVE}" "${BOOST_URL}"
echo "${BOOST_SHA256} ${BOOST_DEST_ARCHIVE}" | sha256sum -c
tar -xzf "${BOOST_DEST_ARCHIVE}" -C /tmp

# Install the Boost library
cd /tmp/boost_1_82_0
./bootstrap.sh --prefix="${INSTALL_PREFIX}"
./b2 install

# Clean up
rm -rf /tmp/boost_1_82_0 "${BOOST_DEST_ARCHIVE}"
