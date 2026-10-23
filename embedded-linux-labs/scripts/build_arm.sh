#!/usr/bin/env bash

set -Eeuo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "${SCRIPT_DIR}/.." && pwd)"

SOURCE="${PROJECT_ROOT}/src/arm_math.c"
BUILD_DIR="${PROJECT_ROOT}/build/arm"

mkdir -p "${BUILD_DIR}"

if ! command -v arm-none-eabi-gcc >/dev/null 2>&1; then
    echo "ERROR: arm-none-eabi-gcc not found." >&2
    exit 1
fi

if [[ ! -f "${SOURCE}" ]]; then
    echo "ERROR: Missing source file: ${SOURCE}" >&2
    exit 1
fi

echo "[ARM] Compiler:"
arm-none-eabi-gcc --version | head -n 1

echo "[ARM] Compiling for Cortex-M4..."

arm-none-eabi-gcc \
    -mcpu=cortex-m4 \
    -mthumb \
    -std=c11 \
    -Wall \
    -Wextra \
    -Wpedantic \
    -Werror \
    -g \
    -O0 \
    -ffreestanding \
    -c "${SOURCE}" \
    -o "${BUILD_DIR}/arm_math.o"

echo "[ARM] Inspecting artifact..."
file "${BUILD_DIR}/arm_math.o"

echo "[ARM] Build completed."
