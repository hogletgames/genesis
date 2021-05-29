#!/usr/bin/env bash

VERSION=${1}
PRIORITY=${2}

USAGE="${0} <version> <priority>"

if [ -z "${VERSION}" ] || [ -z "${PRIORITY}" ]; then
    echo "${USAGE}"
    exit 1
fi

update-alternatives \
    --install /usr/bin/clang          clang          /usr/bin/clang-${VERSION}   ${PRIORITY} \
    --slave   /usr/bin/clang++        clang++        /usr/bin/clang++-${VERSION} \
    --slave   /usr/bin/clang-format   clang-format   /usr/bin/clang-format-${VERSION} \
    --slave   /usr/bin/clang-tidy     clang-tidy     /usr/bin/clang-tidy-${VERSION} \
    --slave   /usr/bin/run-clang-tidy run-clang-tidy /usr/bin/run-clang-tidy-${VERSION}

update-alternatives --config clang
