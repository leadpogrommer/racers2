#!/usr/bin/env python3
"""Verify cached-constraint callback pointer wiring without executing game code."""
import argparse
import struct
from pathlib import Path
from check_renderer_resource_dispatch import verify
from toolchain import DEFAULT_BUILD_DIR, ROOT

TABLES = {
    "base": (0x00531adc, 1, "g_constraintBaseDispatch"),
    "inert": (0x005368e0, 1, "g_inertConstraintDispatch"),
    "contact": (0x00536ca4, 4, "g_contactConstraintDispatch"),
}

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-dir", type=Path, default=DEFAULT_BUILD_DIR)
    args = parser.parse_args()
    try:
        verify(ROOT.parent / "LEGO Racers 2" / "LEGO Racers 2.exe",
               args.build_dir / "LEGO Racers 2.exe", args.build_dir / "legoracers2.map",
               ROOT / "reccmp/constraint-dispatch.csv", TABLES)
    except (KeyError, OSError, ValueError, struct.error) as error:
        parser.exit(1, f"Constraint dispatch verification failed: {error}\n")
    print("Verified cached-constraint tables: 6/6 retail targets and rebuilt callback pointers.")

if __name__ == "__main__":
    main()
