#!/usr/bin/env python3
"""Check the complete renderer resource/buffer callback tables without running the game."""

import argparse
import csv
import hashlib
from pathlib import Path
import struct

from check_application_dispatch import map_symbols, read_table
from setup import ORIGINAL_SHA256
from toolchain import ROOT


TABLES = {
    "texture": (0x005358E4, 10, "g_rendererTextureRecordDispatchTable"),
    "animation": (0x00536394, 5, "g_rendererResourceCacheDispatch"),
    "abstract": (0x0053570C, 5, "g_rendererResourceCacheAbstractDispatch"),
    "vertex": (0x00535488, 44, "g_rendererVertexBufferDispatch"),
    "index": (0x00535538, 7, "g_rendererIndexBufferDispatch"),
    "animation_resource": (0x005356F0, 7, "g_rendererAnimationResourceDispatch"),
}


def verify(original, rebuilt, link_map, inventory):
    original_data = original.read_bytes()
    if hashlib.sha256(original_data).hexdigest() != ORIGINAL_SHA256:
        raise ValueError("Unsupported original executable SHA-256")
    rebuilt_data = rebuilt.read_bytes()
    symbols = map_symbols(link_map)
    with inventory.open(newline="", encoding="utf-8") as stream:
        rows = list(csv.DictReader(stream))
    if len(rows) != sum(spec[1] for spec in TABLES.values()):
        raise ValueError("Inventory must contain every renderer resource/buffer slot")
    failures = []
    for table, (address, count, table_symbol) in TABLES.items():
        slots = [row for row in rows if row["table"] == table]
        if [int(row["offset"], 16) for row in slots] != list(range(0, count * 4, 4)):
            raise ValueError(f"Inventory must list every ordered slot for {table}")
        original_entries = read_table(original_data, address, count)
        rebuilt_entries = read_table(rebuilt_data, symbols[table_symbol], count)
        for index, row in enumerate(slots):
            if original_entries[index] != int(row["original"], 16):
                failures.append(f"{table}+0x{index * 4:02x}: retail target disagrees with inventory")
            if rebuilt_entries[index] != symbols[row["symbol"]]:
                failures.append(f"{table}+0x{index * 4:02x}: rebuilt callback is not {row['symbol']}")
    if failures:
        raise ValueError("\n".join(failures))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, default=ROOT.parent / "LEGO Racers 2" / "LEGO Racers 2.exe")
    parser.add_argument("--build-dir", type=Path, default=ROOT / "build")
    args = parser.parse_args()
    try:
        verify(args.original, args.build_dir / "LEGO Racers 2.exe", args.build_dir / "legoracers2.map",
               ROOT / "reccmp" / "renderer-resource-dispatch.csv")
    except (KeyError, OSError, ValueError, struct.error) as error:
        parser.exit(1, f"Renderer resource dispatch verification failed: {error}\n")
    count = sum(spec[1] for spec in TABLES.values())
    print(f"Verified renderer resource and buffer tables: {count}/{count} retail targets and rebuilt callbacks.")
    print("Callback behavior and instruction accuracy are separate checks.")


if __name__ == "__main__":
    main()
