#!/usr/bin/env python3
"""Lint annotations, generate per-function accuracy reports, and check claims."""

import argparse
from pathlib import Path
import subprocess
import sys

from folded_functions import supplement_report

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-dir", type=Path, default=ROOT / "build")
    parser.add_argument("--address", help="Show assembly diff at an original virtual address")
    args = parser.parse_args()
    build_dir = args.build_dir.resolve()

    def run(module, arguments):
        subprocess.run([sys.executable, "-m", module, *arguments], cwd=build_dir, check=True)

    run("reccmp.tools.decomplint", ["--target", "LEGORACERS2", "--warnfail", str(ROOT / "LEGO Racers 2")])
    if args.address:
        run("reccmp.tools.asmcmp", ["--target", "LEGORACERS2", "--verbose", args.address, "--print-rec-addr"])
        return

    reports = build_dir / "reports"
    reports.mkdir(exist_ok=True)
    prefix = reports / "LEGORACERS2"
    run("reccmp.tools.asmcmp", [
        "--target", "LEGORACERS2", "--nolib", "--print-rec-addr",
        "--json", str(prefix.with_suffix(".json")),
        "--html", str(prefix.with_suffix(".html")),
        "--svg", str(prefix.with_suffix(".svg")),
    ])
    supplement_report(ROOT, build_dir)
    subprocess.run([sys.executable, str(ROOT / "tools" / "check_matches.py"),
                    str(prefix.with_suffix(".json"))], check=True)
    print(f"Reports: {reports}\nCoverage is relative to annotated functions; the whole-game total is unknown.")


if __name__ == "__main__":
    main()
