#!/usr/bin/env python3

import argparse
from datetime import datetime
from pathlib import Path
import sys

import serial


def main():
    parser = argparse.ArgumentParser(
        description="Capture embedded-device serial logs."
    )
    parser.add_argument("--port", required=True,
                        help="Serial port, e.g. /dev/ttyUSB0")
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument("--output-dir", default="logs")
    parser.add_argument("--timeout", type=float, default=1.0)
    args = parser.parse_args()

    if args.baud <= 0:
        parser.error("--baud must be positive")

    output_dir = Path(args.output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    output_path = output_dir / f"serial_{timestamp}.log"

    try:
        with serial.Serial(
            port=args.port,
            baudrate=args.baud,
            timeout=args.timeout,
        ) as port, output_path.open("a", encoding="utf-8") as logfile:
            print(f"[LOGGER] Connected to {args.port} at {args.baud} baud")
            print(f"[LOGGER] Saving to {output_path}")
            print("[LOGGER] Press Ctrl+C to stop.")

            while True:
                raw_line = port.readline()

                if not raw_line:
                    continue

                line = raw_line.decode("utf-8", errors="replace").rstrip()
                host_timestamp = datetime.now().isoformat(timespec="milliseconds")
                record = f"{host_timestamp} {line}"

                print(record)
                logfile.write(record + "\n")
                logfile.flush()

    except KeyboardInterrupt:
        print("\n[LOGGER] Stopped by user.")
    except serial.SerialException as exc:
        print(f"[LOGGER] Serial error: {exc}", file=sys.stderr)
        return 1
    except OSError as exc:
        print(f"[LOGGER] File/system error: {exc}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
