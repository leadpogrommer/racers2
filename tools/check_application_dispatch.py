#!/usr/bin/env python3
"""Verify both complete application dispatch tables in the original and rebuild.

This checks binary wiring, not the semantic correctness of callback bodies.
ABI bridges and original method bodies retain separate instruction measurements.
"""

import argparse
import csv
import hashlib
from pathlib import Path
import re
import struct

from inspect_binary import pe_headers, rva_offset, u32
from setup import ORIGINAL_SHA256
from toolchain import ROOT


def map_symbols(path):
    symbols = {}
    # VC6 emits an ANSI map, and legacy import archives can contribute
    # non-UTF-8 bytes to member names. Decode one byte per character so the
    # ASCII symbol/address columns remain intact without dropping input.
    for line in path.read_text(encoding="latin-1").splitlines():
        match = re.match(r"\s+[0-9a-fA-F]{4}:[0-9a-fA-F]+\s+(\S+)\s+([0-9a-fA-F]{8})\b", line)
        if not match:
            continue
        decorated, address = match.groups()
        name = decorated.split("@@", 1)[0].lstrip("?_")
        symbols[name] = int(address, 16)
    return symbols


def read_table(binary, address, count=38):
    _, optional, sections = pe_headers(binary)
    image_base = u32(binary, optional + 28)
    offset = rva_offset(sections, address - image_base)
    return struct.unpack_from(f"<{count}I", binary, offset)


def verify(original, rebuilt, link_map, inventory):
    original_data = original.read_bytes()
    if hashlib.sha256(original_data).hexdigest() != ORIGINAL_SHA256:
        raise ValueError("Unsupported original executable SHA-256")
    rebuilt_data = rebuilt.read_bytes()
    symbols = map_symbols(link_map)
    with inventory.open(encoding="utf-8", newline="") as stream:
        rows = list(csv.DictReader(stream))
    if [int(row["slot"], 16) for row in rows] != list(range(0, 0x98, 4)):
        raise ValueError("Dispatch inventory must contain exactly 38 ordered slots")

    failures = []
    for table, address, symbol in (
        ("primary", 0x00534994, "g_applicationPrimaryDispatch"),
        ("exit", 0x00534a2c, "g_applicationExitDispatch"),
    ):
        original_entries = read_table(original_data, address)
        rebuilt_entries = read_table(rebuilt_data, symbols[symbol])
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
               ROOT / "reccmp" / "application-dispatch.csv")
    except (KeyError, OSError, ValueError, struct.error) as error:
        parser.exit(1, f"Application dispatch verification failed: {error}\n")
    print("Verified both application tables: 76/76 original targets and rebuilt callback pointers.")
    print("Callback body equivalence and instruction accuracy are separate checks.")


if __name__ == "__main__":
    main()
