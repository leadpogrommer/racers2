#!/usr/bin/env python3
"""Configure DirectX 8 headers and import libraries for the pinned VC6 build.

This helper emits an ignored JSON configuration. Wine support is opt-in via
``--mode wine``; a genuine SDK can be selected with ``--mode sdk``.
"""

import argparse
import json
import os
from pathlib import Path
import shutil

from toolchain import DEFAULT_TOOLCHAIN, ROOT


WINE_HEADERS = ("basetsd.h", "d3d8.h", "d3d8caps.h", "d3d8types.h", "dinput.h", "dsound.h", "dplay8.h", "dpaddr.h", "dplobby8.h")
DX_LIBRARIES = ("d3d8.lib", "dinput8.lib", "dsound.lib", "dxguid.lib")
DEFAULT_WINE_INCLUDE = Path("/usr/include/wine/windows")
DEFAULT_WINE_LIB = Path("/usr/lib/wine/i386-windows")


def _find_case_insensitive(directory, name):
    directory = Path(directory)
    if not directory.is_dir():
        return None
    target = name.lower()
    for child in directory.iterdir():
        if child.name.lower() == target:
            return child
    return None


def _require_files(directory, names, description):
    missing = [name for name in names if _find_case_insensitive(directory, name) is None]
    if missing:
        raise ValueError(f"{description} is missing: {', '.join(missing)} ({directory})")


def _prepare_wine_overlay(source_dir, overlay_dir):
    _require_files(source_dir, WINE_HEADERS, "Wine DirectX headers")
    overlay_dir.mkdir(parents=True, exist_ok=True)
    for name in WINE_HEADERS:
        source = _find_case_insensitive(source_dir, name).resolve()
        destination = overlay_dir / name
        if destination.is_symlink() and destination.resolve() == source:
            continue
        if destination.exists() or destination.is_symlink():
            destination.unlink()
        try:
            destination.symlink_to(source)
        except OSError:
            # Some Windows checkouts do not permit symlinks. This copy remains
            # in the ignored generated directory, never in the source tree.
            shutil.copy2(source, destination)


def _sdk_paths(sdk_root):
    sdk_root = Path(sdk_root).expanduser().resolve()
    include_dir = sdk_root / "Include"
    _require_files(include_dir, ("d3d8.h", "dinput.h", "dsound.h", "dplay8.h", "dpaddr.h", "dplobby8.h"), "DirectX SDK headers")

    library_dir = None
    for candidate in (sdk_root / "Lib" / "x86", sdk_root / "Lib", sdk_root / "lib" / "x86"):
        if all(_find_case_insensitive(candidate, library) is not None for library in DX_LIBRARIES):
            library_dir = candidate.resolve()
            break
    if library_dir is None:
        raise ValueError(f"DirectX SDK import libraries not found under {sdk_root / 'Lib'}")
    return include_dir.resolve(), library_dir


def configure(mode, output_dir, toolchain, sdk_root=None, wine_include=None, wine_lib=None, d3dx8_library=None):
    toolchain = Path(toolchain).expanduser().resolve()
    vc_include = toolchain / "VC98" / "Include"
    vc_lib = toolchain / "VC98" / "Lib"
    if not vc_include.is_dir() or not vc_lib.is_dir():
        raise ValueError(f"VC98 include/library directories not found under {toolchain}")

    output_dir = Path(output_dir).expanduser().resolve()
    if mode == "sdk":
        sdk_root = sdk_root or os.environ.get("DXSDK_DIR") or os.environ.get("DXSDK_ROOT")
        if not sdk_root:
            raise ValueError("--sdk-root or DXSDK_DIR/DXSDK_ROOT is required for --mode sdk")
        include_dir, library_dir = _sdk_paths(sdk_root)
        config = {
            "mode": "sdk",
            "include_dirs": [str(include_dir), str(vc_include)],
            "library_dirs": [str(library_dir), str(vc_lib)],
            "libraries": list(DX_LIBRARIES),
            "defines": [],
            "link_options": [],
        }
    elif mode == "wine":
        source_dir = Path(wine_include or DEFAULT_WINE_INCLUDE).expanduser().resolve()
        library_dir = Path(wine_lib or DEFAULT_WINE_LIB).expanduser().resolve()
        archive_names = tuple("lib" + name[:-4] + ".a" for name in DX_LIBRARIES)
        _require_files(library_dir, archive_names, "Wine PE/i386 DirectX import archives")
        overlay_dir = output_dir / "wine-include-overlay"
        _prepare_wine_overlay(source_dir, overlay_dir)
        config = {
            "mode": "wine",
            "include_dirs": [str(overlay_dir), str(vc_include)],
            "library_dirs": [str(library_dir), str(vc_lib)],
            "libraries": list(archive_names),
            "defines": ["LR2_USE_WINE_DX8"],
            "link_options": ["/OPT:NOREF"],
            "wine_notices": str(source_dir),
        }
    else:
        raise ValueError(f"unsupported DirectX mode: {mode}")

    # D3DX8 is a separate static SDK library. Wine's D3D8 import archive does
    # not provide it. Keep this external input explicit in both build routes.
    if d3dx8_library is None and mode == "sdk":
        d3dx8_library = _find_case_insensitive(library_dir, "d3dx8.lib")
    if d3dx8_library is not None:
        d3dx8_library = Path(d3dx8_library).expanduser().resolve()
        if not d3dx8_library.is_file():
            raise ValueError(f"D3DX8 static library not found: {d3dx8_library}")
        directory = str(d3dx8_library.parent)
        if directory not in config["library_dirs"]:
            config["library_dirs"].insert(0, directory)
        config["libraries"].append(d3dx8_library.name)

    config_path = output_dir / "directx8-config.json"
    output_dir.mkdir(parents=True, exist_ok=True)
    config_path.write_text(json.dumps(config, indent=2) + "\n", encoding="utf-8")
    return config_path, config


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--mode", required=True, choices=("wine", "sdk"))
    parser.add_argument("--output-dir", type=Path, default=ROOT / "build-dx8-config")
    parser.add_argument("--toolchain", type=Path, default=Path(os.environ.get("MSVC6_ROOT", DEFAULT_TOOLCHAIN)))
    parser.add_argument("--sdk-root", type=Path, help="DirectX SDK root; defaults to DXSDK_DIR or DXSDK_ROOT")
    parser.add_argument("--wine-include", type=Path, help="Wine Windows header directory")
    parser.add_argument("--wine-lib", type=Path, help="Wine PE/i386 import archive directory")
    parser.add_argument("--d3dx8-library", type=Path, help="Genuine DirectX 8 SDK retail static d3dx8.lib")
    args = parser.parse_args()
    try:
        path, config = configure(
            args.mode, args.output_dir, args.toolchain, args.sdk_root, args.wine_include, args.wine_lib,
            args.d3dx8_library
        )
    except (OSError, ValueError) as error:
        parser.exit(1, f"DirectX configuration failed: {error}\n")
    print(f"Wrote {path}")
    print(json.dumps(config, indent=2))


if __name__ == "__main__":
    main()
