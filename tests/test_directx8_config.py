"""Check that both builders receive an explicit external D3DX8 dependency."""

import json
from pathlib import Path
import sys
from tempfile import TemporaryDirectory
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from directx8 import configure, DX_LIBRARIES, WINE_HEADERS


class DirectXConfigurationTests(unittest.TestCase):
    def make_inputs(self, root):
        toolchain = root / "vc6"
        (toolchain / "VC98" / "Include").mkdir(parents=True)
        (toolchain / "VC98" / "Lib").mkdir()
        headers = root / "headers"
        libraries = root / "imports"
        headers.mkdir()
        libraries.mkdir()
        for name in WINE_HEADERS:
            (headers / name).touch()
        for name in DX_LIBRARIES:
            (libraries / ("lib" + name[:-4] + ".a")).touch()
        return toolchain, headers, libraries

    def test_explicit_d3dx8_is_in_shared_json(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            toolchain, headers, libraries = self.make_inputs(root)
            d3dx8 = root / "external SDK" / "d3dx8.lib"
            d3dx8.parent.mkdir()
            d3dx8.touch()
            path, _ = configure("wine", root / "config", toolchain,
                                wine_include=headers, wine_lib=libraries,
                                d3dx8_library=d3dx8)
            emitted = json.loads(path.read_text())
            self.assertIn(str(d3dx8.parent), emitted["library_dirs"])
            self.assertIn("d3dx8.lib", emitted["libraries"])
            self.assertIn("libd3d8.a", emitted["libraries"])

    def test_missing_explicit_d3dx8_fails_before_config_is_written(self):
        with TemporaryDirectory() as directory:
            root = Path(directory)
            toolchain, headers, libraries = self.make_inputs(root)
            with self.assertRaisesRegex(ValueError, "D3DX8 static library not found"):
                configure("wine", root / "config", toolchain,
                          wine_include=headers, wine_lib=libraries,
                          d3dx8_library=root / "missing.lib")
            self.assertFalse((root / "config" / "directx8-config.json").exists())


if __name__ == "__main__":
    unittest.main()
