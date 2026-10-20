#!/usr/bin/env python3
"""Check movie service pointers and the three game-owned SDK overrides."""

import argparse
import hashlib
from pathlib import Path
import re
import struct

from check_application_dispatch import map_symbols, read_table
from setup import ORIGINAL_SHA256
from toolchain import DEFAULT_BUILD_DIR, ROOT


def verify(original, rebuilt, link_map):
    original_data = original.read_bytes()
    if hashlib.sha256(original_data).hexdigest() != ORIGINAL_SHA256:
        raise ValueError("Unsupported original executable SHA-256")
    rebuilt_data = rebuilt.read_bytes()
    symbols = map_symbols(link_map)
    service = read_table(rebuilt_data, symbols["g_moviePlayerDispatch"], 3)
    retail = read_table(original_data, 0x0053496c, 3)
    for slot, (target, name) in enumerate((
        (0x0049a3e0, "MoviePlayerPlay"),
        (0x00479030, "MoviePlayerNoOp"),
        (0x00473140, "MoviePlayerZero"),
    )):
        if retail[slot] != target or service[slot] != symbols[name]:
            raise ValueError(f"Movie service slot {slot} disagrees with the recovered dispatch")

    # This class has five interface tables. Use the complete decorated table
    # name so the CUnknown table cannot alias another interface in our parser.
    table = None
    for line in link_map.read_text(encoding="latin-1").splitlines():
        match = re.match(r"\s+[0-9a-fA-F]{4}:[0-9a-fA-F]+\s+"
                         r"\?\?_7MovieTextureRenderer@@6BCUnknown@@@\s+([0-9a-fA-F]{8})\b", line)
        if match:
            table = int(match[1], 16)
            break
    if table is None:
        raise ValueError("Missing MovieTextureRenderer CUnknown vtable symbol")
    for offset, target, name in (
        (0x90, 0x00499ac0, "SetMediaType@MovieTextureRenderer"),
        (0xac, 0x00499d30, "DoRenderSample@MovieTextureRenderer"),
        (0xb0, 0x00499a70, "CheckMediaType@MovieTextureRenderer"),
    ):
        retail = read_table(original_data, 0x0053489c + offset, 1)[0]
        actual = read_table(rebuilt_data, table + offset, 1)[0]
        if retail != target or actual != symbols[name]:
            raise ValueError(f"Movie renderer override +0x{offset:x} disagrees with {name}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, default=ROOT.parent / "LEGO Racers 2" / "LEGO Racers 2.exe")
    parser.add_argument("--build-dir", type=Path, default=DEFAULT_BUILD_DIR)
    args = parser.parse_args()
    try:
        verify(args.original, args.build_dir / "LEGO Racers 2.exe", args.build_dir / "legoracers2.map")
    except (KeyError, OSError, ValueError, struct.error) as error:
        parser.exit(1, f"Movie dispatch verification failed: {error}\n")
    print("Verified all three movie service slots and the three game-owned SDK renderer overrides.")
    print("Inherited SDK callbacks and callback behavior are separate checks.")


if __name__ == "__main__":
    main()
