#!/usr/bin/env python3
"""Check all mouse/keyboard game-dispatch pointers without executing the game."""

import argparse
import csv
import hashlib
from pathlib import Path
import struct

from check_application_dispatch import map_symbols, read_table
from setup import ORIGINAL_SHA256
from toolchain import ROOT


def verify(original, rebuilt, link_map, inventory):
    original_data = original.read_bytes()
    if hashlib.sha256(original_data).hexdigest() != ORIGINAL_SHA256:
        raise ValueError("Unsupported original executable SHA-256")
    rebuilt_data = rebuilt.read_bytes()
    symbols = map_symbols(link_map)
    with inventory.open(encoding="utf-8", newline="") as stream:
        rows = list(csv.DictReader(stream))
    if [int(row["slot"], 16) for row in rows] != list(range(0, 0x64, 4)):
        raise ValueError("Input inventory must contain exactly 25 ordered slots")

    failures = []
    for table, address, symbol in (
        ("mouse", 0x00535928, "g_mouseInputDispatch"),
        ("keyboard", 0x0053598c, "g_keyboardInputDispatch"),
    ):
        original_entries = read_table(original_data, address, 25)
        rebuilt_entries = read_table(rebuilt_data, symbols[symbol], 25)
        for index, row in enumerate(rows):
            if original_entries[index] != int(row[table + "_original"], 16):
                failures.append(f"{table}+0x{index * 4:02x}: retail target disagrees with inventory")
            callback = row[table + "_symbol"]
            expected = symbols[callback]
            if rebuilt_entries[index] != expected:
                failures.append(f"{table}+0x{index * 4:02x}: expected {callback} at 0x{expected:08x}")
    if failures:
        raise ValueError("\n".join(failures))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, default=ROOT.parent / "LEGO Racers 2" / "LEGO Racers 2.exe")
    parser.add_argument("--build-dir", type=Path, default=ROOT / "build")
    args = parser.parse_args()
    try:
        verify(args.original, args.build_dir / "LEGO Racers 2.exe", args.build_dir / "legoracers2.map",
               ROOT / "reccmp" / "input-dispatch.csv")
    except (KeyError, OSError, ValueError, struct.error) as error:
        parser.exit(1, f"Input dispatch verification failed: {error}\n")
    print("Verified both input tables: 50/50 original targets and rebuilt callback pointers.")
    print("Callback behavior and instruction accuracy are separate checks.")


if __name__ == "__main__":
    main()
