#!/usr/bin/env bash

VERSION=${1}
PRIORITY=${2}

USAGE="${0} <version> <priority>"

if [ -z "${VERSION}" ] || [ -z "${PRIORITY}" ]; then
    echo "${USAGE}"
    exit 1
fi

update-alternatives \
    --install /usr/bin/gcc gcc /usr/bin/gcc-${VERSION} ${PRIORITY} \
    --slave   /usr/bin/g++ g++ /usr/bin/g++-${VERSION}

update-alternatives --config gcc
