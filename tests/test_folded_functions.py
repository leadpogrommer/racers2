"""Reject false exact claims when linker folding hides a source line."""

import hashlib
from pathlib import Path
import struct
import sys
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import folded_functions as folded


def pe_fixture(code):
    data = bytearray(0x200 + len(code))
    data[:2] = b"MZ"
    struct.pack_into("<I", data, 0x3c, 0x80)
    data[0x80:0x84] = b"PE\0\0"
    struct.pack_into("<H", data, 0x86, 1)
    struct.pack_into("<H", data, 0x94, 0xe0)
    struct.pack_into("<H", data, 0x98, 0x10b)
    struct.pack_into("<I", data, 0x98 + 28, 0x400000)
    data[0x178:0x180] = b".text\0\0\0"
    struct.pack_into("<IIII", data, 0x180, len(code), 0x1000, len(code), 0x200)
    data[0x200:] = code
    return bytes(data)


class FoldedFunctionTests(unittest.TestCase):
    def setUp(self):
        self.original = pe_fixture(bytes.fromhex("8b4114c38b4114c3"))
        self.rebuilt = pe_fixture(bytes.fromhex("8b4114c3"))
        self.aliases = [
            {"address": "00401000", "symbol": "?GetterA@@QAEIXZ", "bytes": "8b4114c3", "name": "GetterA"},
            {"address": "00401004", "symbol": "?GetterB@@QAEIXZ", "bytes": "8b4114c3", "name": "GetterB"},
        ]
        self.link_map = " 0001:00000000 ?GetterA@@QAEIXZ 00401000 f a.obj\n 0001:00000000 ?GetterB@@QAEIXZ 00401000 f b.obj\n"
        self.sizes = {0x401000: 4, 0x401004: 4}

    def verify(self, **overrides):
        args = dict(original=self.original, rebuilt=self.rebuilt, link_map=self.link_map,
                    aliases=self.aliases, sizes=self.sizes)
        args.update(overrides)
        with patch.object(folded, "ORIGINAL_SHA256", hashlib.sha256(self.original).hexdigest()):
            return folded.verified_aliases(**args)

    def test_two_original_bodies_can_share_one_verified_rebuilt_address(self):
        rows = self.verify()
        self.assertEqual([row["address"] for row in rows], ["0x401000", "0x401004"])
        self.assertEqual({row["recomp"] for row in rows}, {"0x401000"})
        self.assertTrue(all(row["matching"] == 1 and not row.get("stub") for row in rows))

    def test_changed_rebuilt_instruction_is_rejected(self):
        with self.assertRaisesRegex(ValueError, "Rebuilt folded body differs"):
            self.verify(rebuilt=pe_fixture(bytes.fromhex("8b4118c3")))

    def test_prefix_extent_cannot_claim_complete_function(self):
        with self.assertRaisesRegex(ValueError, "complete folded body extent"):
            self.verify(sizes={0x401000: 8, 0x401004: 4})

    def test_wrong_original_and_missing_or_ambiguous_symbols_are_rejected(self):
        with self.assertRaisesRegex(ValueError, "SHA-256"):
            self.verify(original=self.original + b"different")
        with self.assertRaises(KeyError):
            self.verify(link_map="")
        with self.assertRaisesRegex(ValueError, "Ambiguous"):
            self.verify(link_map=self.link_map + " 0001:00000004 ?GetterA@@QAEIXZ 00401004 f c.obj\n")

    def test_virtual_only_pe_address_is_rejected(self):
        with self.assertRaisesRegex(ValueError, "not backed"):
            self.verify(link_map=self.link_map.replace("00401000 f", "00402000 f"))


if __name__ == "__main__":
    unittest.main()
