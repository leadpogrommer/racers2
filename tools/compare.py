#!/usr/bin/env python3
"""Lint annotations, generate per-function accuracy reports, and check claims."""

import argparse
from pathlib import Path
import re
import subprocess
import sys

from folded_functions import supplement_report
from function_inventory import load_inventory
from toolchain import DEFAULT_BUILD_DIR, ROOT


def is_upstream_summary(line):
    plain = re.sub(r"\x1b\[[0-9;]*m", "", line).strip()
    return plain.startswith(("Implemented:", "Accuracy:", "Progress:")) or bool(
        re.fullmatch(r"\d+ functions are aligned \(.+\)", plain))


def run_comparison(arguments, build_dir):
    command = [sys.executable, "-m", "reccmp.tools.asmcmp", *arguments]
    # Upstream prints before complete folded-body verification. Keep per-function
    # output and errors, then print one summary from the final verified report.
    with subprocess.Popen(command, cwd=build_dir, stdout=subprocess.PIPE, text=True, bufsize=1) as process:
        for line in process.stdout:
            if not is_upstream_summary(line):
                print(line, end="", flush=True)
        if process.wait():
            raise subprocess.CalledProcessError(process.returncode, command)


def summary_lines(report, inventory):
    from reccmp.compare.report import report_function_accuracy

    inventory.require_addresses(ent.orig_addr for ent in report.entities.values() if ent.is_function())
    if report.function_count != inventory.total:
        raise ValueError("Report denominator differs from the original function inventory")
    implemented, _, effective = report_function_accuracy(report)
    return [
        f"Original binary: {inventory.total:,} inventoried functions (includes libraries and thunks; discovery may grow).",
        f"Implemented:  {implemented / inventory.total * 100:.2f}%  ({implemented} / {inventory.total})",
        f"Accuracy:     {effective / (implemented or 1) * 100:.2f}%",
        f"Progress:     {effective / inventory.total * 100:.2f}%",
    ]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-dir", type=Path, default=DEFAULT_BUILD_DIR)
    parser.add_argument("--address", help="Show assembly diff at an original virtual address")
    args = parser.parse_args()
    build_dir = args.build_dir.resolve()

    def run(module, arguments):
        subprocess.run([sys.executable, "-m", module, *arguments], cwd=build_dir, check=True)

    run("reccmp.tools.decomplint", ["--target", "LEGORACERS2", "--warnfail", str(ROOT / "LEGO Racers 2")])
    if args.address:
        run("reccmp.tools.asmcmp", ["--target", "LEGORACERS2", "--verbose", args.address, "--print-rec-addr"])
        return

    inventory = load_inventory()
    reports = build_dir / "reports"
    reports.mkdir(exist_ok=True)
    prefix = reports / "LEGORACERS2"
    run_comparison([
        "--target", "LEGORACERS2", "--nolib", "--print-rec-addr",
        "--total", str(inventory.total),
        "--json", str(prefix.with_suffix(".json")),
        "--html", str(prefix.with_suffix(".html")),
        "--svg", str(prefix.with_suffix(".svg")),
    ], build_dir)
    report = supplement_report(ROOT, build_dir, inventory)
    summary = summary_lines(report, inventory)
    subprocess.run([sys.executable, str(ROOT / "tools" / "check_matches.py"),
                    str(prefix.with_suffix(".json"))], check=True)
    print("\n" + "\n".join(summary))
    print(f"Reports: {reports}")


if __name__ == "__main__":
    main()
