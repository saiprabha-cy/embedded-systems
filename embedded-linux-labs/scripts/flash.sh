#!/usr/bin/env bash

set -Eeuo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd -- "${SCRIPT_DIR}/.." && pwd)"

FIRMWARE="${PROJECT_ROOT}/build/telemetry_sim"

usage()
{
    echo "Usage: $0 <target> [--dry-run]"
    echo "Example: $0 stm32f4 --dry-run"
}

if [[ $# -lt 1 || $# -gt 2 ]]; then
    usage >&2
    exit 2
fi

TARGET="$1"
MODE="${2:-}"

if [[ "${TARGET}" != "stm32f4" ]]; then
    echo "ERROR: Unsupported target: ${TARGET}" >&2
    exit 2
fi

if [[ "${MODE}" != "" && "${MODE}" != "--dry-run" ]]; then
    usage >&2
    exit 2
fi

if [[ ! -f "${FIRMWARE}" ]]; then
    echo "ERROR: Firmware artifact missing: ${FIRMWARE}" >&2
    echo "Build the project first." >&2
    exit 1
fi

if [[ "${MODE}" == "--dry-run" ]]; then
    echo "[DRY RUN] Target: ${TARGET}"
    echo "[DRY RUN] Artifact: ${FIRMWARE}"
    echo "[DRY RUN] No hardware was accessed."
    exit 0
fi

if ! command -v STM32_Programmer_CLI >/dev/null 2>&1; then
    echo "ERROR: STM32_Programmer_CLI is not installed or not in PATH." >&2
    echo "Install/configure the vendor tool before flashing." >&2
    exit 1
fi

echo "This script requires a target-specific flash command."
echo "The current host simulator is not a valid STM32 firmware image." >&2
echo "Refusing to flash." >&2
exit 1
