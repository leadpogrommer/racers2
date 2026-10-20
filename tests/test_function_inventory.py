"""Keep report coverage tied to original entries rather than annotated rows."""

import csv
import hashlib
import json
from pathlib import Path
import sys
import struct
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from compare import is_upstream_summary, summary_lines
import function_inventory as inventory_tools
from function_inventory import FunctionInventory, load_inventory, parse_rows
from setup import ORIGINAL_SHA256
from reccmp.compare.report import deserialize_reccmp_report


class FunctionInventoryTests(unittest.TestCase):
    def write_inventory(self, root, text, **overrides):
        directory = root / "reccmp"
        directory.mkdir(exist_ok=True)
        data = text.encode("utf-8")
        rows = list(csv.DictReader(text.splitlines()))
        metadata = {
            "format": 1,
            "original_sha256": ORIGINAL_SHA256,
            "inventory_sha256": hashlib.sha256(data).hexdigest(),
            "function_count": len(rows),
            "thunk_count": sum(row["thunk"] == "true" for row in rows),
            "exhaustive": False,
            "executable_ranges": [["00401000", "00402000"]],
        }
        metadata.update(overrides)
        (directory / "function-inventory.csv").write_bytes(data)
        (directory / "function-inventory.json").write_text(json.dumps(metadata))

    def test_checked_in_inventory_loads_without_retail_or_ghidra_files(self):
        inventory = load_inventory()
        self.assertGreater(inventory.total, 2800)
        # Both ends of a verified CRT wrapper/body pair must contribute.
        inventory.require_addresses([0x401000, 0x401010, 0x4c32b0, 0x51a08f])
        self.assertFalse(inventory.metadata["exhaustive"])

    def test_duplicate_and_unsorted_addresses_are_rejected(self):
        header = "address,name,thunk\n"
        for rows in ("00401000,A,false\n00401000,B,true\n", "00401010,B,false\n00401000,A,false\n"):
            with self.subTest(rows=rows), self.assertRaisesRegex(ValueError, "unique and sorted"):
                parse_rows(header + rows)

    def test_wrong_binary_stale_checksum_and_nonexecutable_entries_are_rejected(self):
        text = "address,name,thunk\n00401000,A,false\n"
        for override, error in (({"original_sha256": "0" * 64}, "SHA-256"),
                                ({"inventory_sha256": "0" * 64}, "checksum"),
                                ({"function_count": 100}, "counts"),
                                ({"executable_ranges": [["00500000", "00501000"]]}, "outside executable")):
            with self.subTest(override=override), tempfile.TemporaryDirectory() as temp:
                root = Path(temp)
                self.write_inventory(root, text, **override)
                with self.assertRaisesRegex(ValueError, error):
                    load_inventory(root)

    def final_report(self):
        return deserialize_reccmp_report(json.dumps({
            "format": 1, "file": "fixture.exe", "timestamp": 0, "function_count": 10,
            "data": [
                {"address": "0x401000", "recomp": "0x402000", "name": "Exact", "matching": 1, "type": 1},
                {"address": "0x401010", "recomp": "0x402000", "name": "Folded", "matching": 1, "type": 1, "folded": True},
                {"address": "0x401020", "recomp": "0x402010", "name": "Unfinished", "matching": 1, "type": 1, "stub": True},
            ],
        }))

    def test_refresh_checks_pe_identity_and_requires_review_before_removing_starts(self):
        binary = bytearray(0x240)
        binary[:2] = b"MZ"
        struct.pack_into("<I", binary, 0x3c, 0x80)
        binary[0x80:0x84] = b"PE\0\0"
        struct.pack_into("<H", binary, 0x86, 1)
        struct.pack_into("<H", binary, 0x94, 0xe0)
        struct.pack_into("<H", binary, 0x98, 0x10b)
        struct.pack_into("<I", binary, 0x98 + 28, 0x400000)
        binary[0x178:0x180] = b".text\0\0\0"
        struct.pack_into("<IIII", binary, 0x180, 0x40, 0x1000, 0x40, 0x200)
        struct.pack_into("<I", binary, 0x178 + 36, 0x20000000)
        with tempfile.TemporaryDirectory() as temp, patch.object(
                inventory_tools, "ORIGINAL_SHA256", hashlib.sha256(binary).hexdigest()):
            root = Path(temp)
            (root / "reccmp").mkdir()
            (root / "reccmp/function-sizes.csv").write_text("address,type,size\n00401000,function,5\n")
            original = root / "original.exe"
            original.write_bytes(binary)
            export = root / "export.csv"
            header = "address,name,body_bytes,maximum_address,thunk,external\n"
            kept = "00401000,Wrapper,5,00401004,true,false\n00401010,Body,1,00401010,false,false\n"
            extra = "00401020,Callback,1,00401020,false,false\n"
            external = "77abcdef,ImportedAPI,0,77abcdef,false,true\n"
            export.write_text(header + kept + extra + external)
            inventory = inventory_tools.refresh_inventory(export, original, root)
            self.assertEqual(inventory.total, 3)
            self.assertEqual(inventory.metadata["thunk_count"], 1)
            export.write_text(header + kept)
            with self.assertRaisesRegex(ValueError, "remove 1 inventoried"):
                inventory_tools.refresh_inventory(export, original, root)
            self.assertEqual(load_inventory(root).total, 3)
            self.assertEqual(inventory_tools.refresh_inventory(export, original, root, allow_removals=True).total, 2)
            original.write_bytes(binary + b"wrong version")
            with self.assertRaisesRegex(ValueError, "SHA-256"):
                inventory_tools.refresh_inventory(export, original, root)

    def test_summary_uses_original_inventory_and_final_folded_rows(self):
        inventory = FunctionInventory(frozenset(range(0x401000, 0x4010a0, 0x10)), {})
        lines = summary_lines(self.final_report(), inventory)
        self.assertIn("Implemented:  20.00%  (2 / 10)", lines)
        self.assertIn("Accuracy:     100.00%", lines)
        self.assertIn("Progress:     20.00%", lines)

    def test_count_or_address_mismatch_cannot_fall_back_to_annotated_total(self):
        report = self.final_report()
        with self.assertRaisesRegex(ValueError, "missing from inventory"):
            summary_lines(report, FunctionInventory(frozenset([0x401000]), {}))
        with self.assertRaisesRegex(ValueError, "denominator differs"):
            summary_lines(report, FunctionInventory(frozenset([0x401000, 0x401010, 0x401020]), {}))

    def test_only_the_preverification_upstream_summary_is_filtered(self):
        for line in ("Implemented: 22.09% (604 / 2734)", "Accuracy: 99.83%", "Progress: 22.05%",
                     "\x1b[32mImplemented: 22.09%\x1b[0m", "12 functions are aligned (0.5%)"):
            self.assertTrue(is_upstream_summary(line), line)
        for line in ("0x00401000: Test function Accuracy: 100%", "WARNING: unmatched symbol", "Traceback (most recent call last):"):
            self.assertFalse(is_upstream_summary(line), line)


if __name__ == "__main__":
    unittest.main()
