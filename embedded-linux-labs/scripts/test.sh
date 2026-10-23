#!/usr/bin/env bash

set -Eeuo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "${SCRIPT_DIR}/.." && pwd)"

BUILD_DIR="${PROJECT_ROOT}/build"
LOG_DIR="${PROJECT_ROOT}/logs"

mkdir -p "${BUILD_DIR}" "${LOG_DIR}"

TEST_BINARY="${BUILD_DIR}/test_telemetry"
TEST_LOG="${LOG_DIR}/test_telemetry.log"

echo "[TEST] Compiling unit tests..."

gcc \
    -std=c11 \
    -Wall \
    -Wextra \
    -Wpedantic \
    -Werror \
    -g \
    -O0 \
    -I"${PROJECT_ROOT}/include" \
    "${PROJECT_ROOT}/src/telemetry.c" \
    "${PROJECT_ROOT}/tests/test_telemetry.c" \
    -o "${TEST_BINARY}"

echo "[TEST] Executing tests..."

if "${TEST_BINARY}" 2>&1 | tee "${TEST_LOG}"; then
    echo "[TEST] PASS"
else
    echo "[TEST] FAIL — see ${TEST_LOG}" >&2
    exit 1
fi
