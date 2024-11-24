#!/usr/bin/env bash

CLANG_FORMAT_BIN="${CLANG_FORMAT_BIN:-clang-format}"
EXIT_CODE=0
FIX=false

PATHS_TO_SRC=(apps examples include src tests)
SRC_FILES=$(find "${PATHS_TO_SRC[@]}" -name "*.h" -o -name "*.cpp")

while [ -n "${1}" ]; do
    case ${1} in
    -f|--fix)
        FIX=true
        ;;
    --clang-format-bin)
        shift
        CLANG_FORMAT_BIN=$1
        ;;
    *)
        echo "Not supported arg: ${1}"
        exit 1
    esac
    shift
done

for FILE in ${SRC_FILES}; do
    echo "Checking: ${FILE}"
    ${CLANG_FORMAT_BIN} --dry-run --Werror "${FILE}"

    if [ $? -ne 0 ]; then
        if ${FIX}; then
            echo "Fixing: ${FILE}"
            ${CLANG_FORMAT_BIN} --style=file -i "${FILE}"
        else
            EXIT_CODE=1
        fi
    fi
done

exit ${EXIT_CODE}
