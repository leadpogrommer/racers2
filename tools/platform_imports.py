"""Locate SDK import libraries absent from the preserved VC6 installation."""

import os
from pathlib import Path


def shfolder_library(toolchain, override=None):
    selected = override or os.environ.get("SHFOLDER_LIB")
    if selected:
        library = Path(selected).resolve()
        if not library.is_file():
            raise ValueError(f"SHFOLDER import library does not exist: {library}")
        return library

    candidates = [Path(toolchain) / "VC98" / "Lib" / "shfolder.lib"]
    for directory in os.environ.get("LIB", "").split(";"):
        if directory:
            candidates.append(Path(directory) / "shfolder.lib")
    if os.name != "nt":
        candidates.append(Path("/usr/lib/wine/i386-windows/libshfolder.a"))
    for library in candidates:
        if library.is_file():
            return library.resolve()
    raise ValueError("Set SHFOLDER_LIB to an x86 SHFOLDER import library; see docs/work-userdatapath.md")
