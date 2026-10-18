#!/usr/bin/env python3
"""Verify all recovered startup scene callback pointers without running the game."""

import argparse
from pathlib import Path
import struct

from check_renderer_resource_dispatch import verify
from toolchain import ROOT

TABLES = {
    "bodypart": (0x0052f584, 21, "g_bodypartManagerDispatch"),
    "glyph": (0x0052f57c, 2, "g_overlayGlyphManagerDispatch"),
    "screen": (0x005325b8, 21, "g_screenPrinterDispatch"),
}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, default=ROOT.parent / "LEGO Racers 2" / "LEGO Racers 2.exe")
    parser.add_argument("--build-dir", type=Path, default=ROOT / "build")
    args = parser.parse_args()
    try:
        verify(args.original, args.build_dir / "LEGO Racers 2.exe", args.build_dir / "legoracers2.map",
               ROOT / "reccmp" / "scene-dispatch.csv", TABLES)
    except (KeyError, OSError, ValueError, struct.error) as error:
        parser.exit(1, f"Scene dispatch verification failed: {error}\n")
    print("Verified startup scene tables: 44/44 retail targets and rebuilt callback pointers.")
    print("Callback behavior and instruction accuracy are separate checks.")


if __name__ == "__main__":
    main()
