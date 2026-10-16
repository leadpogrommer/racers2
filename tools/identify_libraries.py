#!/usr/bin/env python3
"""Find archive object functions whose relocation-masked bytes match retail code.

This tool emits review candidates only. It does not annotate source or decide
whether a function should be classified as library code.
"""

from __future__ import annotations

import argparse
import csv
from dataclasses import dataclass
from pathlib import Path
import struct
import sys


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_ARCHIVE = ROOT / ".tools/msvc6-sp5/VC98/Lib/LIBCMT.LIB"
DEFAULT_RETAIL = ROOT.parent / "LEGO Racers 2" / "LEGO Racers 2.exe"
DEFAULT_SIZES = ROOT / "data/ghidra-export/discovered-functions.csv"
DEFAULT_OUTPUT = ROOT / "data/library-candidates.csv"

# Width in bytes of supported i386 COFF relocations. Unknown relocations make
# that object function ineligible instead of silently weakening the comparison.
RELOC_WIDTH = {
    0x0001: 2,  # IMAGE_REL_I386_DIR16
    0x0002: 2,  # IMAGE_REL_I386_REL16
    0x0006: 4,  # IMAGE_REL_I386_DIR32
    0x0007: 4,  # IMAGE_REL_I386_DIR32NB
    0x000A: 2,  # IMAGE_REL_I386_SECTION
    0x000B: 4,  # IMAGE_REL_I386_SECREL
    0x0014: 4,  # IMAGE_REL_I386_REL32
}


@dataclass
class Section:
    name: str
    raw_size: int
    raw_pointer: int
    reloc_pointer: int
    reloc_count: int


@dataclass
class ObjectFunction:
    member: str
    symbol: str
    body: bytes
    relocations: tuple[tuple[int, int], ...]


def read_c_string(data: bytes, offset: int) -> bytes:
    if offset < 0 or offset >= len(data):
        return b""
    end = data.find(b"\0", offset)
    if end < 0:
        end = len(data)
    return data[offset:end]


def archive_members(path: Path):
    data = path.read_bytes()
    if not data.startswith(b"!<arch>\n"):
        raise ValueError(f"Not a COFF archive: {path}")

    long_names = b""
    position = 8
    while position < len(data):
        if position + 60 > len(data):
            raise ValueError(f"Truncated archive member header at 0x{position:x}")
        header = data[position : position + 60]
        if header[58:60] != b"`\n":
            raise ValueError(f"Invalid archive member header at 0x{position:x}")
        raw_name = header[:16].decode("ascii", "replace").strip()
        try:
            member_size = int(header[48:58].decode("ascii").strip())
        except ValueError as error:
            raise ValueError(f"Invalid archive member size at 0x{position:x}") from error
        if member_size < 0:
            raise ValueError(f"Invalid archive member size at 0x{position:x}")
        start = position + 60
        member = data[start : start + member_size]
        if len(member) != member_size:
            raise ValueError(f"Truncated archive member {raw_name!r}")

        if raw_name == "//":
            long_names = member
        elif raw_name not in ("/", "/SYM64/"):
            if raw_name.startswith("/"):
                try:
                    name_offset = int(raw_name[1:])
                except ValueError as error:
                    raise ValueError(f"Invalid long-name offset {raw_name!r}") from error
                if name_offset < 0 or name_offset >= len(long_names):
                    raise ValueError(f"Long-name offset out of range: {raw_name!r}")
                end = long_names.find(b"\0", name_offset)
                slash_newline = long_names.find(b"/\n", name_offset)
                if end < 0 or (0 <= slash_newline < end):
                    end = slash_newline if slash_newline >= 0 else len(long_names)
                name = long_names[name_offset:end].decode("ascii", "replace")
            else:
                name = raw_name.rstrip("/")
            if member[:2] == b"\x4c\x01":
                yield name, member

        position = start + member_size + (member_size & 1)


def coff_functions(member_name: str, data: bytes):
    if len(data) < 20:
        return
    machine, section_count, _, symbol_pointer, symbol_count, optional_size, _ = struct.unpack_from(
        "<HHIIIHH", data, 0
    )
    if machine != 0x014C or symbol_pointer == 0:
        return

    section_offset = 20 + optional_size
    sections: list[Section] = []
    for index in range(section_count):
        offset = section_offset + index * 40
        header = data[offset : offset + 40]
        if len(header) != 40:
            return
        name = header[:8].split(b"\0", 1)[0].decode("ascii", "replace")
        raw_size, raw_pointer, reloc_pointer = struct.unpack_from("<III", header, 16)
        reloc_count = struct.unpack_from("<H", header, 32)[0]
        if raw_pointer + raw_size > len(data):
            return
        if reloc_count and reloc_pointer + reloc_count * 10 > len(data):
            return
        sections.append(Section(name, raw_size, raw_pointer, reloc_pointer, reloc_count))

    if symbol_pointer + symbol_count * 18 > len(data):
        return
    string_pointer = symbol_pointer + symbol_count * 18
    if string_pointer + 4 > len(data):
        return
    string_size = struct.unpack_from("<I", data, string_pointer)[0]
    if string_size < 4 or string_pointer + string_size > len(data):
        return
    strings = data[string_pointer : string_pointer + string_size]

    symbols = []
    index = 0
    while index < symbol_count:
        offset = symbol_pointer + index * 18
        record = data[offset : offset + 18]
        if len(record) != 18:
            return
        if record[:4] == b"\0\0\0\0":
            name_offset = struct.unpack_from("<I", record, 4)[0]
            symbol_name = read_c_string(strings, name_offset).decode("ascii", "replace")
        else:
            symbol_name = record[:8].split(b"\0", 1)[0].decode("ascii", "replace")
        value = struct.unpack_from("<I", record, 8)[0]
        section_number = struct.unpack_from("<h", record, 12)[0]
        symbol_type = struct.unpack_from("<H", record, 14)[0]
        storage_class = record[16]
        aux_count = record[17]
        if index + 1 + aux_count > symbol_count:
            return
        aux_record = data[offset + 18 : offset + 36] if aux_count else b""
        aux_size = struct.unpack_from("<I", aux_record, 4)[0] if aux_record else 0
        symbols.append((symbol_name, value, section_number, symbol_type, storage_class, aux_size))
        index += 1 + aux_count

    by_section: dict[int, list[tuple]] = {}
    for symbol in symbols:
        name, value, section_number, symbol_type, storage_class, aux_size = symbol
        # COFF derived type 2 denotes a function. Accept external and static
        # definitions; undefined symbols and section auxiliaries are excluded.
        if (
            0 < section_number <= len(sections)
            and symbol_type & 0x20
            and storage_class in (2, 3)
        ):
            by_section.setdefault(section_number, []).append(symbol)

    for section_number, section_symbols in by_section.items():
        section = sections[section_number - 1]
        starts = sorted({symbol[1] for symbol in section_symbols})
        for name, start, _, _, _, aux_size in section_symbols:
            if aux_size:
                body_size = aux_size
            else:
                later_starts = [candidate for candidate in starts if candidate > start]
                body_size = (min(later_starts) if later_starts else section.raw_size) - start
            if body_size <= 0 or start + body_size > section.raw_size:
                continue

            body = data[section.raw_pointer + start : section.raw_pointer + start + body_size]
            relocations = []
            unsupported = False
            for relocation_index in range(section.reloc_count):
                relocation_offset = section.reloc_pointer + relocation_index * 10
                if relocation_offset + 10 > len(data):
                    unsupported = True
                    break
                virtual_address, symbol_index, relocation_type = struct.unpack_from(
                    "<IIH", data, relocation_offset
                )
                if symbol_index >= symbol_count:
                    if start <= virtual_address < start + body_size:
                        unsupported = True
                        break
                    continue
                width = RELOC_WIDTH.get(relocation_type)
                if width is None:
                    # Relocations outside the function do not affect it.
                    if start <= virtual_address < start + body_size:
                        unsupported = True
                        break
                    continue
                if start <= virtual_address < start + body_size:
                    relocations.append((virtual_address - start, width))
            if not unsupported:
                yield ObjectFunction(member_name, name, body, tuple(relocations))


def retail_text(path: Path):
    data = path.read_bytes()
    if data[:2] != b"MZ":
        raise ValueError(f"Not a PE executable: {path}")
    pe_offset = struct.unpack_from("<I", data, 0x3C)[0]
    if data[pe_offset : pe_offset + 4] != b"PE\0\0":
        raise ValueError(f"Invalid PE signature: {path}")
    section_count = struct.unpack_from("<H", data, pe_offset + 6)[0]
    optional_size = struct.unpack_from("<H", data, pe_offset + 20)[0]
    optional_offset = pe_offset + 24
    magic = struct.unpack_from("<H", data, optional_offset)[0]
    if magic != 0x10B:
        raise ValueError("Only 32-bit PE images are supported")
    image_base = struct.unpack_from("<I", data, optional_offset + 28)[0]
    section_offset = optional_offset + optional_size
    sections = []
    for index in range(section_count):
        offset = section_offset + index * 40
        header = data[offset : offset + 40]
        name = header[:8].split(b"\0", 1)[0].decode("ascii", "replace")
        virtual_size, virtual_address, raw_size, raw_pointer = struct.unpack_from("<IIII", header, 8)
        sections.append((name, virtual_size, virtual_address, raw_size, raw_pointer))
    return data, image_base, sections


def function_sizes(path: Path):
    result = {}
    with path.open(newline="", encoding="utf-8-sig") as stream:
        for row in csv.DictReader(stream):
            try:
                address = int(row["address"], 16)
                size_field = row.get("size") or row.get("body_bytes")
                size = int(size_field, 10)
            except (KeyError, TypeError, ValueError):
                continue
            if size > 0:
                result[address] = size
    return result


def section_bytes(image, image_base, sections, address, size):
    rva = address - image_base
    for name, virtual_size, virtual_address, raw_size, raw_pointer in sections:
        section_extent = max(virtual_size, raw_size)
        if name == ".text" and virtual_address <= rva and rva + size <= virtual_address + section_extent:
            raw_offset = raw_pointer + rva - virtual_address
            if raw_offset + size <= len(image) and raw_offset + size <= raw_pointer + raw_size:
                return image[raw_offset : raw_offset + size]
    return None


def relocation_mask(length: int, relocations):
    mask = bytearray(length)
    for offset, width in relocations:
        end = min(length, offset + width)
        if offset < 0 or offset >= length:
            continue
        mask[offset:end] = b"\1" * (end - offset)
    return mask


def longest_unmasked_run(mask):
    longest = current = 0
    for masked in mask:
        if masked:
            current = 0
        else:
            current += 1
            longest = max(longest, current)
    return longest


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--archive", type=Path, default=DEFAULT_ARCHIVE)
    parser.add_argument("--retail", type=Path, default=DEFAULT_RETAIL)
    parser.add_argument("--sizes", type=Path, default=DEFAULT_SIZES)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument(
        "--targets",
        help="comma-separated retail VAs; by default scans addresses in discovered-functions.csv",
    )
    parser.add_argument("--min-unmasked", type=int, default=32, help="minimum compared bytes (default: 32)")
    parser.add_argument("--min-run", type=int, default=8, help="minimum contiguous unmasked run (default: 8)")
    args = parser.parse_args()

    sizes = function_sizes(args.sizes)
    if args.targets:
        targets = [int(token.strip(), 0) for token in args.targets.split(",") if token.strip()]
    else:
        targets = sorted(sizes)

    image, image_base, sections = retail_text(args.retail)
    retail_bodies = {}
    for address in targets:
        size = sizes.get(address)
        if size is None:
            print(f"No function size for {address:#010x}; skipped", file=sys.stderr)
            continue
        body = section_bytes(image, image_base, sections, address, size)
        if body is not None:
            retail_bodies[address] = (size, body)

    object_functions = []
    for member_name, member_data in archive_members(args.archive):
        object_functions.extend(coff_functions(member_name, member_data))

    matches = []
    by_size = {}
    for function in object_functions:
        by_size.setdefault(len(function.body), []).append(function)

    for address, (size, retail_body) in retail_bodies.items():
        for function in by_size.get(size, ()):
            mask = relocation_mask(size, function.relocations)
            unmasked_count = size - sum(mask)
            if unmasked_count < args.min_unmasked or longest_unmasked_run(mask) < args.min_run:
                continue
            differences = [
                offset
                for offset, masked in enumerate(mask)
                if not masked and retail_body[offset] != function.body[offset]
            ]
            if differences:
                continue
            masked_bytes = sum(mask)
            raw_differences = sum(a != b for a, b in zip(retail_body, function.body))
            matches.append(
                {
                    "target_va": f"0x{address:08x}",
                    "archive_member": function.member,
                    "object_symbol": function.symbol,
                    "body_size": size,
                    "masked_bytes": masked_bytes,
                    "unmasked_bytes": unmasked_count,
                    "longest_unmasked_run": longest_unmasked_run(mask),
                    "relocation_masked_identical": "true",
                    "raw_differences": raw_differences,
                }
            )

    args.output.parent.mkdir(parents=True, exist_ok=True)
    fields = [
        "target_va",
        "archive_member",
        "object_symbol",
        "body_size",
        "masked_bytes",
        "unmasked_bytes",
        "longest_unmasked_run",
        "relocation_masked_identical",
        "raw_differences",
    ]
    with args.output.open("w", newline="", encoding="utf-8") as stream:
        writer = csv.DictWriter(stream, fieldnames=fields)
        writer.writeheader()
        writer.writerows(sorted(matches, key=lambda row: (row["target_va"], row["archive_member"], row["object_symbol"])))

    print(f"Scanned {len(retail_bodies)} retail functions against {len(object_functions)} COFF functions")
    print(f"Wrote {len(matches)} relocation-masked candidates to {args.output}")
    print("Candidates require manual source/caller review; matching is not raw-byte reccmp accuracy.")


if __name__ == "__main__":
    main()
