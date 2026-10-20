#!/usr/bin/env python3
"""Load or refresh the original executable's function inventory for reporting."""

import argparse
import csv
from dataclasses import dataclass
import hashlib
import io
import json
from pathlib import Path
import re

from inspect_binary import pe_headers, u16, u32
from setup import ORIGINAL_SHA256
from toolchain import ROOT


@dataclass(frozen=True)
class FunctionInventory:
    addresses: frozenset[int]
    metadata: dict

    @property
    def total(self):
        return len(self.addresses)

    def require_addresses(self, addresses):
        missing = sorted(set(addresses) - self.addresses)
        if missing:
            values = ", ".join(f"0x{address:08x}" for address in missing)
            raise ValueError(f"Original functions missing from inventory: {values}. "
                             "Refresh it with tools/function_inventory.py; do not use the annotated count.")


def parse_rows(text):
    rows = list(csv.DictReader(io.StringIO(text)))
    previous = -1
    for row in rows:
        if not re.fullmatch(r"[0-9a-f]{8}", row["address"]):
            raise ValueError("Inventory addresses must be lowercase, eight-digit original VAs")
        address = int(row["address"], 16)
        if address <= previous:
            raise ValueError("Inventory addresses must be unique and sorted")
        if row["thunk"] not in ("true", "false") or not row["name"]:
            raise ValueError("Invalid inventory function metadata")
        previous = address
    if not rows:
        raise ValueError("Empty original function inventory")
    return rows


def load_inventory(root=ROOT):
    directory = root / "reccmp"
    data = (directory / "function-inventory.csv").read_bytes()
    metadata = json.loads((directory / "function-inventory.json").read_text())
    if metadata["format"] != 1 or metadata["original_sha256"] != ORIGINAL_SHA256:
        raise ValueError("Unsupported function inventory format or original SHA-256")
    if hashlib.sha256(data).hexdigest() != metadata["inventory_sha256"]:
        raise ValueError("Function inventory checksum differs; refresh CSV and metadata together")
    rows = parse_rows(data.decode("utf-8"))
    addresses = frozenset(int(row["address"], 16) for row in rows)
    if len(rows) != metadata["function_count"] or sum(row["thunk"] == "true" for row in rows) != metadata["thunk_count"]:
        raise ValueError("Function inventory counts differ from its records")
    if metadata["exhaustive"] is not False:
        raise ValueError("Automatic discovery does not establish an exhaustive function count")
    for address in addresses:
        if not any(int(start, 16) <= address < int(end, 16)
                   for start, end in metadata["executable_ranges"]):
            raise ValueError(f"Inventory function outside executable sections: 0x{address:08x}")
    return FunctionInventory(addresses, metadata)


def refresh_inventory(export, original, root=ROOT, allow_removals=False):
    binary = original.read_bytes()
    if hashlib.sha256(binary).hexdigest() != ORIGINAL_SHA256:
        raise ValueError("Unsupported original executable SHA-256")
    pe, optional, sections = pe_headers(binary)
    base = u32(binary, optional + 28)
    section_table = optional + u16(binary, pe + 20)
    executable_ranges = []
    for index, section in enumerate(sections):
        if u32(binary, section_table + index * 40 + 36) & 0x20000000:
            start = base + section["rva"]
            end = start + min(section["raw_size"], section["virtual_size"])
            executable_ranges.append((start, end))
    source = export.read_bytes()
    discovered = list(csv.DictReader(io.StringIO(source.decode("utf-8"))))
    rows = []
    for row in discovered:
        if row["external"] == "true":
            continue
        if row["external"] != "false":
            raise ValueError("Invalid Ghidra external flag")
        address = int(row["address"], 16)
        if not any(start <= address < end for start, end in executable_ranges):
            raise ValueError(f"Non-executable original function: 0x{address:08x}")
        rows.append({key: row[key] for key in ("address", "name", "thunk")})
    stream = io.StringIO(newline="")
    writer = csv.DictWriter(stream, fieldnames=("address", "name", "thunk"), lineterminator="\n")
    writer.writeheader()
    writer.writerows(sorted(rows, key=lambda row: row["address"]))
    data = stream.getvalue().encode("utf-8")
    rows = parse_rows(data.decode("utf-8"))
    addresses = frozenset(int(row["address"], 16) for row in rows)
    if (root / "reccmp/function-inventory.csv").exists() and not allow_removals:
        removed = load_inventory(root).addresses - addresses
        if removed:
            raise ValueError(f"Refresh would remove {len(removed)} inventoried starts. "
                             "Review boundary changes before using --allow-removals.")
    with (root / "reccmp/function-sizes.csv").open(newline="") as sizes:
        FunctionInventory(addresses, {}).require_addresses(int(row["address"], 16) for row in csv.DictReader(sizes))
    metadata = {
        "format": 1,
        "original_sha256": ORIGINAL_SHA256,
        "inventory_sha256": hashlib.sha256(data).hexdigest(),
        "function_count": len(rows),
        "thunk_count": sum(row["thunk"] == "true" for row in rows),
        "exhaustive": False,
        "scope": "Unique internal executable function entries, including linked libraries, compiler helpers and thunks; excludes external imports and inlined functions.",
        "source": "Ghidra analysis after SeedFunctionInventory.java, exported by ExportWorkRanges.java",
        "source_export_sha256": hashlib.sha256(source).hexdigest(),
        "executable_ranges": [[f"{start:08x}", f"{end:08x}"] for start, end in executable_ranges],
    }
    (root / "reccmp/function-inventory.csv").write_bytes(data)
    (root / "reccmp/function-inventory.json").write_text(json.dumps(metadata, indent=2) + "\n")
    return load_inventory(root)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--refresh", type=Path, help="Refresh from a seeded Ghidra discovered-functions.csv export")
    parser.add_argument("--original", type=Path, default=ROOT.parent / "LEGO Racers 2/LEGO Racers 2.exe")
    parser.add_argument("--allow-removals", action="store_true", help="Accept reviewed removals from the existing entry inventory")
    args = parser.parse_args()
    inventory = refresh_inventory(args.refresh, args.original, allow_removals=args.allow_removals) if args.refresh else load_inventory()
    print(f"Original functions: {inventory.total:,} inventoried ({inventory.metadata['thunk_count']:,} thunks). "
          "Includes libraries; discovery is not proven exhaustive.")


if __name__ == "__main__":
    main()
