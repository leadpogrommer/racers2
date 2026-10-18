#!/usr/bin/env python3
"""Verify the spatial list and registry callback tables without running the game."""

import argparse
from pathlib import Path
import struct

from check_renderer_resource_dispatch import verify
from toolchain import ROOT


TABLES = {
    "list": (0x00537528, 2, "g_spatialListDispatch"),
    "registry": (0x00537084, 2, "g_spatialRegistryDispatch"),
    "x_axis": (0x005375cc, 2, "g_spatialXAxisDispatch"),
    "z_axis": (0x005375d4, 2, "g_spatialZAxisDispatch"),
}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, default=ROOT.parent / "LEGO Racers 2" / "LEGO Racers 2.exe")
    parser.add_argument("--build-dir", type=Path, default=ROOT / "build")
    args = parser.parse_args()
    try:
        verify(args.original, args.build_dir / "LEGO Racers 2.exe", args.build_dir / "legoracers2.map",
               ROOT / "reccmp" / "spatial-list-dispatch.csv", TABLES)
    except (KeyError, OSError, ValueError, struct.error) as error:
        parser.exit(1, f"Spatial list dispatch verification failed: {error}\n")
    print("Verified spatial list/registry tables: 8/8 retail targets and rebuilt callback pointers.")
    print("Callback behavior and instruction accuracy are separate checks.")


if __name__ == "__main__":
    main()
