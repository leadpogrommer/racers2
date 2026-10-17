"""Regression coverage for the static dispatch verifier's VC6 map reader."""

from pathlib import Path
import sys
from tempfile import TemporaryDirectory
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from check_application_dispatch import map_symbols


class LinkMapTests(unittest.TestCase):
    def test_non_utf8_archive_names_preserve_symbol_addresses(self):
        with TemporaryDirectory() as directory:
            link_map = Path(directory) / "game.map"
            link_map.write_bytes(
                b" 0001:00000010 _ImportedFunction@4 00401010 f libdx:\xd0\x8e\r\n"
                b"\xf0\xff\r\n"
                b" 0002:00000020 ?g_applicationPrimaryDispatch@@3UApplicationDispatch@@B "
                b"00412020 input.obj\r\n"
            )
            self.assertEqual(map_symbols(link_map), {
                "ImportedFunction@4": 0x00401010,
                "g_applicationPrimaryDispatch": 0x00412020,
            })


if __name__ == "__main__":
    unittest.main()
