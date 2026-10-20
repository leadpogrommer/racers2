#!/usr/bin/env python3
"""Verify all recovered audio service/provider callback pointers without running the game."""

import argparse
from pathlib import Path
import struct

from check_renderer_resource_dispatch import verify
from toolchain import DEFAULT_BUILD_DIR, ROOT

TABLES = {
    "service": (0x005359f4, 42, "g_audioServiceOperationalDispatch"),
    "service_base": (0x00535a9c, 42, "g_audioServiceBaseDispatch"),
    "provider_3d": (0x00536f50, 15, "g_threeDimensionalDirectSoundProviderDispatch"),
    "provider_base": (0x00536f8c, 15, "g_audioBackendBaseDispatch"),
    "provider_stereo": (0x00536fcc, 15, "g_stereoDirectSoundProviderDispatch"),
    "provider_mono": (0x0053700c, 15, "g_monoDirectSoundProviderDispatch"),
}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, default=ROOT.parent / "LEGO Racers 2" / "LEGO Racers 2.exe")
    parser.add_argument("--build-dir", type=Path, default=DEFAULT_BUILD_DIR)
    args = parser.parse_args()
    try:
        verify(args.original, args.build_dir / "LEGO Racers 2.exe", args.build_dir / "legoracers2.map",
               ROOT / "reccmp" / "audio-dispatch.csv", TABLES)
    except (KeyError, OSError, ValueError, struct.error) as error:
        parser.exit(1, f"Audio dispatch verification failed: {error}\n")
    print("Verified audio service/provider tables: 144/144 retail targets and rebuilt callback pointers.")
    print("Callback behavior and instruction accuracy are separate checks.")


if __name__ == "__main__":
    main()
