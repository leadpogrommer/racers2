#!/usr/bin/env python3
"""Compare all input formats, including the SDK mouse adjustment, with retail."""

import argparse
import hashlib
from pathlib import Path
import struct

from check_application_dispatch import map_symbols
from inspect_binary import pe_headers, rva_offset, u32
from setup import ORIGINAL_SHA256
from toolchain import ROOT


def read_format(binary, address):
    _, optional, sections = pe_headers(binary)
    image_base = u32(binary, optional + 28)

    def offset(va):
        return rva_offset(sections, va - image_base)

    header = struct.unpack_from("<6I", binary, offset(address))
    size, stride, flags, data_size, count, objects = header
    if size != 24 or stride != 16 or count > 256:
        raise ValueError(f"Unsupported DirectInput format header at 0x{address:08x}")
    entries = []
    for index in range(count):
        guid, position, kind, object_flags = struct.unpack_from("<4I", binary, offset(objects) + index * stride)
        guid_bytes = binary[offset(guid):offset(guid) + 16] if guid else None
        entries.append((guid_bytes, position, kind, object_flags))
    return header[:5], entries


def verify(original, rebuilt, link_map):
    original_data = original.read_bytes()
    if hashlib.sha256(original_data).hexdigest() != ORIGINAL_SHA256:
        raise ValueError("Unsupported original executable SHA-256")
    rebuilt_data = rebuilt.read_bytes()
    symbols = map_symbols(link_map)
    for address, symbol in (
        (0x0053a9f4, "g_mouseInputDataFormat"),
        (0x0053abfc, "c_dfDIKeyboard"),
        (0x0053ae04, "c_dfDIJoystick"),
    ):
        expected = read_format(original_data, address)
        actual = read_format(rebuilt_data, symbols[symbol])
        if actual != expected:
            raise ValueError(f"{symbol}: linked SDK format differs from retail (header, GUIDs or object entries)")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--original", type=Path, default=ROOT.parent / "LEGO Racers 2" / "LEGO Racers 2.exe")
    parser.add_argument("--build-dir", type=Path, default=ROOT / "build")
    args = parser.parse_args()
    try:
        verify(args.original, args.build_dir / "LEGO Racers 2.exe", args.build_dir / "legoracers2.map")
    except (KeyError, OSError, ValueError, struct.error) as error:
        parser.exit(1, f"DirectInput format verification failed: {error}\n")
    print("Verified mouse, keyboard and joystick formats: headers, GUIDs and all 307 object entries.")


if __name__ == "__main__":
    main()
