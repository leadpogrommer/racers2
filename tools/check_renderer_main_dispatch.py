#!/usr/bin/env python3
"""Verify the renderer's full base, derived and lookup callback tables without running the game."""

import argparse
from pathlib import Path
import struct

from check_renderer_resource_dispatch import verify
from toolchain import ROOT


TABLES = {
    "base": (0x00535728, 103, "g_rendererBaseDispatch"),
    "derived": (0x00535554, 103, "g_rendererDerivedDispatch"),
    "lookup_bucketed": (0x005358d4, 2, "g_rendererBucketedLookupDispatch"),
    "lookup_ordered": (0x005358cc, 2, "g_rendererOrderedLookupDispatch"),
    "lookup_deferred": (0x005358c4, 2, "g_rendererDeferredLookupDispatch"),
}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, default=ROOT.parent / "LEGO Racers 2" / "LEGO Racers 2.exe")
    parser.add_argument("--build-dir", type=Path, default=ROOT / "build")
    args = parser.parse_args()
    try:
        verify(args.original, args.build_dir / "LEGO Racers 2.exe", args.build_dir / "legoracers2.map",
               ROOT / "reccmp" / "renderer-main-dispatch.csv", TABLES)
    except (KeyError, OSError, ValueError, struct.error) as error:
        parser.exit(1, f"Renderer main dispatch verification failed: {error}\n")
    print("Verified renderer base/derived/lookup tables: 212/212 retail targets and rebuilt callback pointers.")
    print("Callback behavior and instruction accuracy are separate checks.")


if __name__ == "__main__":
    main()
