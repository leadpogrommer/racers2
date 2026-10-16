"""Synthetic COFF/archive tests for the library-candidate scanner."""

import struct
import sys
from pathlib import Path
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import identify_libraries as scanner


def coff_object(code, functions, relocations=()):
    """Build a one-section i386 COFF object with external function symbols."""
    section_offset = 20
    relocation_pointer = section_offset + 40
    raw_pointer = relocation_pointer + len(relocations) * 10
    symbol_pointer = raw_pointer + len(code)
    symbol_records = []
    for name, value in functions:
        encoded = name.encode("ascii")
        if len(encoded) > 8:
            raise ValueError("fixture symbol names must fit inline")
        symbol_records.append(
            struct.pack("<8sIhHBB", encoded, value, 1, 0x20, 2, 0)
        )
    symbol_table = b"".join(symbol_records)
    header = struct.pack(
        "<HHIIIHH", 0x014C, 1, 0, symbol_pointer, len(functions), 0, 0
    )
    section = bytearray(40)
    section[:8] = b".text\0\0\0"
    struct.pack_into("<III", section, 16, len(code), raw_pointer, relocation_pointer)
    struct.pack_into("<H", section, 32, len(relocations))
    relocation_data = b"".join(struct.pack("<IIH", *item) for item in relocations)
    return (
        header
        + bytes(section)
        + relocation_data
        + code
        + symbol_table
        + struct.pack("<I", 4)
    )


def archive_member(name, payload):
    encoded_name = (name + "/")[:16].ljust(16).encode("ascii")
    header = (
        encoded_name
        + b"0".ljust(12)
        + b"0".ljust(6)
        + b"0".ljust(6)
        + b"100644".ljust(8)
        + str(len(payload)).encode("ascii").ljust(10)
        + b"`\n"
    )
    return header + payload + (b"\n" if len(payload) & 1 else b"")


class LibraryIdentificationTests(unittest.TestCase):
    def test_unsupported_relocation_excludes_only_affected_function(self):
        data = coff_object(
            bytes(range(16)),
            [("first", 0), ("second", 8)],
            [(2, 0, 0x9999)],
        )

        functions = list(scanner.coff_functions("fixture.obj", data))

        self.assertEqual([function.symbol for function in functions], ["second"])
        self.assertEqual(functions[0].body, bytes(range(8, 16)))

    def test_supported_relocation_masks_only_its_own_bytes(self):
        data = coff_object(
            b"\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b",
            [("target", 0)],
            [(2, 0, 0x0006)],
        )
        function = next(scanner.coff_functions("fixture.obj", data))
        mask = scanner.relocation_mask(len(function.body), function.relocations)

        self.assertEqual(list(mask), [0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0])
        altered_only_in_relocation = bytearray(function.body)
        altered_only_in_relocation[2:6] = b"\xaa\xbb\xcc\xdd"
        self.assertFalse(
            any(
                not masked and function.body[offset] != altered_only_in_relocation[offset]
                for offset, masked in enumerate(mask)
            )
        )
        altered_outside_relocation = bytearray(function.body)
        altered_outside_relocation[6] ^= 0xFF
        self.assertTrue(
            any(
                not masked and function.body[offset] != altered_outside_relocation[offset]
                for offset, masked in enumerate(mask)
            )
        )

    def test_named_function_boundary_stops_before_next_symbol(self):
        data = coff_object(bytes(range(12)), [("first", 0), ("second", 8)])

        functions = list(scanner.coff_functions("fixture.obj", data))

        self.assertEqual(
            [(f.symbol, len(f.body)) for f in functions], [("first", 8), ("second", 4)]
        )
        self.assertEqual(functions[0].body, bytes(range(8)))

    def test_archive_rejects_truncation_and_invalid_headers(self):
        valid = b"!<arch>\n" + archive_member("fixture.obj", b"\x4c\x01")
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "fixture.lib"
            for malformed in (valid[:-1], b"!<arch>\n" + b"x" * 60):
                path.write_bytes(malformed)
                with self.subTest(malformed=malformed[-8:]):
                    with self.assertRaises(ValueError):
                        list(scanner.archive_members(path))

    def test_invalid_symbol_section_index_is_ignored_safely(self):
        data = bytearray(coff_object(bytes(range(8)), [("badsect", 0)]))
        symbol_pointer = struct.unpack_from("<I", data, 8)[0]
        struct.pack_into("<h", data, symbol_pointer + 12, 2)

        self.assertEqual(list(scanner.coff_functions("fixture.obj", bytes(data))), [])


if __name__ == "__main__":
    unittest.main()
