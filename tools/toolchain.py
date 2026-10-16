"""Pinned SP5 toolchain identity, shared by setup and the portable builder."""

from pathlib import Path

from inspect_binary import file_version

ROOT = Path(__file__).resolve().parents[1]
TOOLCHAIN_URL = "https://github.com/archaic-msvc/msvc600_sp5.git"
TOOLCHAIN_COMMIT = "b0b07e29108e2695eb0274c2a377a7b7d7326150"
DEFAULT_TOOLCHAIN = ROOT / ".tools" / "msvc6-sp5"
EXPECTED_VERSIONS = {
    "VC98/Bin/CL.EXE": (12, 0, 8804, 0),
    "VC98/Bin/C1XX.DLL": (12, 0, 8964, 0),
    "VC98/Bin/C2.DLL": (12, 0, 8966, 0),
    "VC98/Bin/LINK.EXE": (6, 0, 8447, 0),
}


def validate(root):
    for name, expected in EXPECTED_VERSIONS.items():
        actual = file_version(root / name)
        if actual != expected:
            raise ValueError(f"{name}: expected {expected}, got {actual}")
    for name in ["VC98/Lib/LIBCMT.LIB", "VC98/Include/WINDOWS.H", "Common/MSDev98/Bin/MSPDB60.DLL"]:
        if not (root / name).is_file():
            raise ValueError(f"Missing toolchain file: {root / name}")
