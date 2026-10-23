#!/usr/bin/env bash

set -Eeuo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"

if [[ "${BUILD_DIR}" != "${PROJECT_ROOT}/build" ]]; then
    echo "ERROR: Unexpected build directory. Refusing to clean." >&2
    exit 1
fi

if [[ -d "${BUILD_DIR}" ]]; then
    echo "[CLEAN] Removing generated build directory..."
    rm -rf -- "${BUILD_DIR}"
else
    echo "[CLEAN] No build directory exists."
fi

mkdir -p "${BUILD_DIR}"

echo "[CLEAN] Build directory recreated."
