#!/usr/bin/env bash

REPO_DIR="$(realpath ${1})"
PATCHES_DIR="$(realpath ${2})"

USAGE="$0 <repository_dir> <patches_dir>"

function apply_patch() {
  local PATCH_FILE="${1}"
  patch -p 1 --forward --ignore-whitespace -d "${REPO_DIR}" < "${PATCH_FILE}"
  return ${?}
}

function check_patch() {
  local PATCH_FILE="${1}"
  patch -p 1 -R --dry-run -d "${REPO_DIR}" < "${PATCH_FILE}"
  return ${?}
}

function main() {
  if [ ! -d "${REPO_DIR}" ] || [ ! -d "${PATCHES_DIR}" ]; then
    echo "${USAGE}"
    exit 1
  fi

  for file in $(ls ${PATCHES_DIR}/*); do
   if ! apply_patch "${file}" && ! check_patch "${file}"; then
     echo "Failed to apply patch"
     exit 1
   fi
  done
}

main
