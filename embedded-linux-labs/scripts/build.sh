#!/usr/bin/env bash

set -Eeuo pipefail

# Resolve the project root from this script's location.
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "${SCRIPT_DIR}/.." && pwd)"

BUILD_DIR="${PROJECT_ROOT}/build"
SOURCE_FILE="${PROJECT_ROOT}/src/main.c"
OUTPUT_FILE="${BUILD_DIR}/telemetry_sim"

mkdir -p "${BUILD_DIR}"

if [[ ! -f "${SOURCE_FILE}" ]]; then
    echo "ERROR: Source file not found: ${SOURCE_FILE}" >&2
    exit 1
fi

echo "[BUILD] Compiling telemetry simulator..."

gcc \
    -std=c11 \
    -Wall \
    -Wextra \
    -Wpedantic \
    -Werror \
    -g \
    -O0 \
    "${SOURCE_FILE}" \
    -o "${OUTPUT_FILE}"

echo "[BUILD] Success: ${OUTPUT_FILE}"
echo "[BUILD] Running program..."
"${OUTPUT_FILE}"
