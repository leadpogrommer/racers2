#!/usr/bin/env python3
"""Build the matching x86 PE + PDB natively on Windows or using Wine on Linux."""

import argparse
import json
import os
from pathlib import Path
import shutil
import subprocess

from toolchain import DEFAULT_TOOLCHAIN, ROOT, validate


def windows_path(path):
    path = Path(path).resolve()
    return str(path) if os.name == "nt" else "Z:" + str(path).replace("/", "\\")


def build(build_dir, toolchain, source_list=None):
    validate(toolchain)
    build_dir.mkdir(parents=True, exist_ok=True)
    wine = []
    env = os.environ.copy()
    bin_dir = toolchain / "VC98" / "Bin"
    pdb_dir = toolchain / "Common" / "MSDev98" / "Bin"
    if os.name == "nt":
        env["PATH"] = os.pathsep.join([str(bin_dir), str(pdb_dir), env["PATH"]])
    else:
        if not shutil.which("wine"):
            raise ValueError("Wine is required on Linux")
        wine = ["wine"]
        env.setdefault("WINEDEBUG", "-all")
        env["WINEPATH"] = ";".join([windows_path(bin_dir), windows_path(pdb_dir), env.get("WINEPATH", "")])
    env["INCLUDE"] = windows_path(toolchain / "VC98" / "Include")
    env["LIB"] = windows_path(toolchain / "VC98" / "Lib")
    # Prevent inherited developer-shell options from silently changing the build.
    for name in ("CL", "_CL_", "LINK", "_LINK_"):
        env.pop(name, None)

    def run(tool, arguments):
        subprocess.run(wine + [str(bin_dir / tool)] + arguments, cwd=build_dir, env=env, check=True)

    source_list = source_list or ROOT / "sources.txt"
    sources = source_list.read_text(encoding="utf-8").splitlines()
    objects = []
    for source in sources:
        if not source.strip():
            continue
        # Include the relative source path to prevent collisions between equal basenames.
        obj = build_dir / "obj" / Path(source).with_suffix(".obj")
        obj.parent.mkdir(parents=True, exist_ok=True)
        run("CL.EXE", [
            "/nologo", "/c", "/W3", "/GX", "/O2", "/Ob1", "/MT", "/Z7", "/DNDEBUG",
            "/DWIN32", "/D_WINDOWS", "/DENABLE_DECOMP_ASSERTS",
            "/I" + windows_path(ROOT / "LEGO Racers 2" / "include"),
            "/I" + windows_path(ROOT / "util"), "/Fo" + windows_path(obj),
            windows_path(ROOT / source),
        ])
        objects.append(windows_path(obj))
    exe = build_dir / "LEGO Racers 2.exe"
    pdb = build_dir / "LEGO Racers 2.pdb"
    run("LINK.EXE", [
        "/nologo", "/machine:I386", "/subsystem:windows,4.0", "/incremental:no",
        # Keep recovered functions measurable before the real callers exist.
        # This changes link retention only; instruction generation is unchanged.
        "/opt:noref,icf", "/debug", "/pdbtype:sept", "/out:" + windows_path(exe),
        "/pdb:" + windows_path(pdb), "/map:" + windows_path(build_dir / "legoracers2.map"),
        *objects, "kernel32.lib", "user32.lib",
    ])
    # Host paths allow Linux reccmp to read Wine-generated artifacts directly.
    config = ("project: " + json.dumps(str(ROOT)) + "\ntargets:\n  LEGORACERS2:\n    path: "
              + json.dumps(str(exe)) + "\n    pdb: " + json.dumps(str(pdb)) + "\n")
    (build_dir / "reccmp-build.yml").write_text(config, encoding="utf-8")
    print(f"Built {exe}\nPDB: {pdb}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-dir", type=Path, default=ROOT / "build")
    parser.add_argument("--source-list", type=Path, help="Alternate source manifest for an isolated worker build")
    parser.add_argument("--toolchain", type=Path, default=Path(os.environ.get("MSVC6_ROOT", DEFAULT_TOOLCHAIN)))
    args = parser.parse_args()
    try:
        build(args.build_dir.resolve(), args.toolchain.resolve(), args.source_list)
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        parser.exit(1, f"Build failed: {error}\n")


if __name__ == "__main__":
    main()
