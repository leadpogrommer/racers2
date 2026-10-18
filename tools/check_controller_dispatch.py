#!/usr/bin/env python3
"""Verify controller polling and system-clock callback pointers without running the game."""

import argparse
from pathlib import Path
import struct

from check_renderer_resource_dispatch import verify
from toolchain import ROOT


TABLES = {
    "clock": (0x00535C74, 8, "g_systemClockDispatch"),
    "controller": (0x00535F84, 4, "g_controllerManagerDispatch"),
}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, default=ROOT.parent / "LEGO Racers 2" / "LEGO Racers 2.exe")
    parser.add_argument("--build-dir", type=Path, default=ROOT / "build")
    args = parser.parse_args()
    try:
        verify(args.original, args.build_dir / "LEGO Racers 2.exe", args.build_dir / "legoracers2.map",
               ROOT / "reccmp" / "controller-dispatch.csv", TABLES)
    except (KeyError, OSError, ValueError, struct.error) as error:
        parser.exit(1, f"Controller/clock dispatch verification failed: {error}\n")
    print("Verified controller/clock tables: 12/12 retail targets and rebuilt callback pointers.")
    print("Callback behavior and instruction accuracy are separate checks.")


if __name__ == "__main__":
    main()
