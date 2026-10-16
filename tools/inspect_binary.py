#!/usr/bin/env python3
"""Inspect PE32 identity, imports and Rich compiler stamps without dependencies."""

import argparse
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import struct


PRODUCTS = {
    0: "Unknown", 1: "Import0", 6: "Cvtres500", 10: "Utc12_C",
    11: "Utc12_CPP", 12: "AliasObj60", 14: "Masm613", 19: "Linker512",
    28: "Utc13_C", 29: "Utc13_CPP", 37: "Linker620", 48: "Utc12_2_C",
}


def u16(data, offset):
    return struct.unpack_from("<H", data, offset)[0]


def u32(data, offset):
    return struct.unpack_from("<I", data, offset)[0]


def rol32(value, shift):
    shift %= 32
    return ((value << shift) | (value >> ((32 - shift) % 32))) & 0xffffffff


def pe_headers(data):
    if data[:2] != b"MZ":
        raise ValueError("Missing DOS header")
    pe = u32(data, 0x3c)
    if data[pe:pe + 4] != b"PE\0\0":
        raise ValueError("Missing PE header")
    optional = pe + 24
    if u16(data, optional) != 0x10b:
        raise ValueError("Expected PE32")
    sections = []
    table = optional + u16(data, pe + 20)
    for index in range(u16(data, pe + 6)):
        offset = table + index * 40
        sections.append({
            "name": data[offset:offset + 8].rstrip(b"\0").decode("ascii"),
            "virtual_size": u32(data, offset + 8),
            "rva": u32(data, offset + 12),
            "raw_size": u32(data, offset + 16),
            "raw_offset": u32(data, offset + 20),
        })
    return pe, optional, sections


def rva_offset(sections, rva):
    for section in sections:
        delta = rva - section["rva"]
        if 0 <= delta < section["raw_size"]:
            return section["raw_offset"] + delta
    raise ValueError(f"RVA 0x{rva:08x} is not backed by file data")


def rich_header(data, pe):
    rich = data.find(b"Rich", 0x40, pe)
    if rich == -1:
        return None
    key = u32(data, rich + 4)
    start = rich - 16
    while start >= 0x40 and u32(data, start) ^ key != 0x536e6144:
        start -= 4
    if start < 0x40 or (rich - start - 16) % 8:
        raise ValueError("Invalid Rich header")
    if any(u32(data, start + delta) ^ key for delta in (4, 8, 12)):
        raise ValueError("Invalid DanS padding")
    checksum = start
    for index, byte in enumerate(data[:start]):
        if not 0x3c <= index < 0x40:
            checksum += rol32(byte, index)
    records = []
    for offset in range(start + 16, rich, 8):
        compid = u32(data, offset) ^ key
        count = u32(data, offset + 4) ^ key
        checksum += rol32(compid, count)
        product = compid >> 16
        records.append({
            "comp_id": f"0x{compid:08x}", "product_id": product,
            "product": PRODUCTS.get(product, "Unknown"),
            "build": compid & 0xffff, "count": count,
        })
    return {
        "offset": f"0x{rich:x}", "xor_key": f"0x{key:08x}",
        "checksum_valid": (checksum & 0xffffffff) == key, "records": records,
    }


def file_version(path):
    """Read VS_FIXEDFILEINFO from a toolchain file's resource section."""
    data = Path(path).read_bytes()
    _, _, sections = pe_headers(data)
    resource = next(section for section in sections if section["name"] == ".rsrc")
    start = resource["raw_offset"]
    offset = data.find(b"\xbd\x04\xef\xfe\x00\x00\x01\x00", start, start + resource["raw_size"])
    if offset < 0:
        raise ValueError(f"No VS_FIXEDFILEINFO in {path}")
    ms, ls = struct.unpack_from("<II", data, offset + 8)
    return (ms >> 16, ms & 0xffff, ls >> 16, ls & 0xffff)


def inspect(path):
    path = Path(path)
    data = path.read_bytes()
    pe, optional, sections = pe_headers(data)
    imports = []
    import_rva = u32(data, optional + 104)
    if import_rva:
        offset = rva_offset(sections, import_rva)
        while any(data[offset:offset + 20]):
            name = rva_offset(sections, u32(data, offset + 12))
            imports.append(data[name:data.index(b"\0", name)].decode("ascii"))
            offset += 20
    image_base = u32(data, optional + 28)
    return {
        "filename": path.name, "size": len(data),
        "sha256": hashlib.sha256(data).hexdigest(), "md5": hashlib.md5(data).hexdigest(),
        "machine": f"0x{u16(data, pe + 4):04x}",
        "timestamp_utc": datetime.fromtimestamp(u32(data, pe + 8), timezone.utc).isoformat(),
        "image_base": f"0x{image_base:08x}",
        "entry_point": f"0x{image_base + u32(data, optional + 16):08x}",
        "linker_version": f"{data[optional + 2]}.{data[optional + 3]}",
        "sections": sections, "imports": imports, "rich": rich_header(data, pe),
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("binary", type=Path)
    args = parser.parse_args()
    print(json.dumps(inspect(args.binary), indent=2))


if __name__ == "__main__":
    main()
