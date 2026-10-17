#!/usr/bin/env python3
"""Extract PE resource entries into a Windows .res file without changing the input."""

import argparse
import hashlib
from pathlib import Path
import struct

from inspect_binary import inspect, pe_headers, rva_offset, u16, u32


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_ORIGINAL = ROOT.parent / "LEGO Racers 2" / "LEGO Racers 2.exe"
ORIGINAL_SHA256 = "6ed248b0ee8e52669ca93f0a2d7eb4c4c2c7ed477550a5f488bfe029270d854d"
IMAGE_FILE_MACHINE_I386 = 0x014C
IMAGE_RESOURCE_NAME_IS_STRING = 0x80000000
IMAGE_RESOURCE_DATA_IS_DIRECTORY = 0x80000000
RESOURCE_MEMORY_FLAGS = 0x1030


def _checked_slice(data, offset, size, limit, description):
    if offset < 0 or size < 0 or offset + size > limit or offset + size > len(data):
        raise ValueError(f"{description} is outside the resource directory or input file")
    return data[offset : offset + size]


def _resource_name(data, base_offset, directory_size, value):
    if value & IMAGE_RESOURCE_NAME_IS_STRING:
        relative = value & 0x7FFFFFFF
        prefix = _checked_slice(
            data, base_offset + relative, 2, base_offset + directory_size, "resource name length"
        )
        length = struct.unpack("<H", prefix)[0]
        raw = _checked_slice(
            data,
            base_offset + relative + 2,
            length * 2,
            base_offset + directory_size,
            "resource name",
        )
        return raw.decode("utf-16le", errors="surrogatepass")
    identifier = value & 0xFFFF
    if value != identifier:
        raise ValueError(f"Resource identifier 0x{value:08x} does not fit in a WORD")
    return identifier


def _directory_entries(data, base_offset, directory_size, relative_offset):
    directory_offset = base_offset + relative_offset
    header = _checked_slice(
        data, directory_offset, 16, base_offset + directory_size, "resource directory header"
    )
    named_count, id_count = struct.unpack_from("<HH", header, 12)
    count = named_count + id_count
    raw_entries = _checked_slice(
        data,
        directory_offset + 16,
        count * 8,
        base_offset + directory_size,
        "resource directory entries",
    )
    entries = []
    for index in range(count):
        name_value, child_value = struct.unpack_from("<II", raw_entries, index * 8)
        name = _resource_name(data, base_offset, directory_size, name_value)
        child_offset = child_value & 0x7FFFFFFF
        is_directory = bool(child_value & IMAGE_RESOURCE_DATA_IS_DIRECTORY)
        entries.append((name, child_offset, is_directory))
    return entries


def _resource_tree(data, optional_header, sections):
    if u16(data, optional_header + 92) < 3:
        raise ValueError("PE has no resource data-directory entry")
    resource_rva = u32(data, optional_header + 96 + 2 * 8)
    resource_size = u32(data, optional_header + 96 + 2 * 8 + 4)
    if resource_rva == 0 or resource_size == 0:
        raise ValueError("PE has an empty resource directory")
    base_offset = rva_offset(sections, resource_rva)
    if base_offset + resource_size > len(data):
        raise ValueError("PE resource directory exceeds the input file")

    resources = []
    for type_name, type_offset, type_is_directory in _directory_entries(
        data, base_offset, resource_size, 0
    ):
        if not type_is_directory:
            raise ValueError("Resource type entry has no child directory")
        for name, name_offset, name_is_directory in _directory_entries(
            data, base_offset, resource_size, type_offset
        ):
            if not name_is_directory:
                raise ValueError("Resource name entry has no language directory")
            for language, data_offset, data_is_directory in _directory_entries(
                data, base_offset, resource_size, name_offset
            ):
                if data_is_directory:
                    raise ValueError("Resource language entry has an unexpected nested directory")
                if not isinstance(language, int) or language > 0xFFFF:
                    raise ValueError(f"Resource language must be a numeric WORD, got {language!r}")
                entry_offset = base_offset + data_offset
                raw_entry = _checked_slice(
                    data, entry_offset, 16, base_offset + resource_size, "resource data entry"
                )
                payload_rva, payload_size, codepage, reserved = struct.unpack("<IIII", raw_entry)
                payload_offset = rva_offset(sections, payload_rva)
                payload = _checked_slice(data, payload_offset, payload_size, len(data), "resource payload")
                resources.append((type_name, name, language, payload, codepage, reserved))
    return resources


def _identifier_bytes(identifier):
    if isinstance(identifier, int):
        if not 0 <= identifier <= 0xFFFF:
            raise ValueError(f"Resource identifier {identifier} does not fit in a WORD")
        return struct.pack("<HH", 0xFFFF, identifier)
    if not isinstance(identifier, str):
        raise TypeError(f"Unsupported resource identifier: {identifier!r}")
    return identifier.encode("utf-16le", errors="surrogatepass") + b"\0\0"


def _res_record(resource):
    resource_type, name, language, payload, _codepage, _reserved = resource
    if not 0 <= language <= 0xFFFF:
        raise ValueError(f"Resource language {language} does not fit in a WORD")

    header = bytearray(struct.pack("<II", len(payload), 0))
    header.extend(_identifier_bytes(resource_type))
    header.extend(_identifier_bytes(name))
    header.extend(b"\0" * ((-len(header)) & 3))
    header.extend(struct.pack("<IHHII", 0, RESOURCE_MEMORY_FLAGS, language, 0, 0))
    struct.pack_into("<I", header, 4, len(header))
    record = header + payload
    record.extend(b"\0" * ((-len(record)) & 3))
    return record


def build_res(resources):
    # Windows .res files begin with a special null resource header.
    result = bytearray(struct.pack("<IIHHHHIHHII", 0, 32, 0xFFFF, 0, 0xFFFF, 0, 0, 0, 0, 0, 0))
    for resource in resources:
        result.extend(_res_record(resource))
    return bytes(result)


def extract(original, output):
    original = Path(original).expanduser().resolve()
    output = Path(output).expanduser().resolve()
    if original == output:
        raise ValueError("Output path must not overwrite the original executable")

    identity = inspect(original)
    if identity["sha256"] != ORIGINAL_SHA256:
        raise ValueError(f"Unsupported original SHA-256: {identity['sha256']}")
    if identity["machine"] != f"0x{IMAGE_FILE_MACHINE_I386:04x}":
        raise ValueError(f"Expected an x86 PE32 executable, got machine {identity['machine']}")

    data = original.read_bytes()
    _pe, optional_header, sections = pe_headers(data)
    resources = _resource_tree(data, optional_header, sections)
    contents = build_res(resources)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(contents)
    return {
        "original_sha256": identity["sha256"],
        "resource_count": len(resources),
        "payload_bytes": sum(len(resource[3]) for resource in resources),
        "res_bytes": len(contents),
        "res_sha256": hashlib.sha256(contents).hexdigest(),
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, default=DEFAULT_ORIGINAL)
    parser.add_argument("--output", type=Path, default=ROOT / "build-resources" / "retail.res")
    args = parser.parse_args()
    try:
        result = extract(args.original, args.output)
    except (OSError, ValueError, UnicodeError) as error:
        parser.exit(1, f"Resource extraction failed: {error}\n")
    print(f"Wrote {args.output.resolve()}")
    print(f"Extracted {result['resource_count']} resources ({result['payload_bytes']} payload bytes)")
    print(f".res size: {result['res_bytes']} bytes; SHA-256: {result['res_sha256']}")
    print(f"Verified original SHA-256: {result['original_sha256']}")


if __name__ == "__main__":
    main()
