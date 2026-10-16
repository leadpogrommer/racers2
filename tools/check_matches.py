#!/usr/bin/env python3
"""Fail if a FUNCTION claim is missing from a reccmp report or is below 100%."""

import argparse
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
MARKER = re.compile(r"^\s*//\s*FUNCTION:\s+LEGORACERS2\s+(0x[0-9a-fA-F]+)\b", re.MULTILINE)


def claimed_addresses(source_root):
    return {
        int(match, 16)
        for path in source_root.rglob("*")
        if path.suffix in (".cpp", ".h")
        for match in MARKER.findall(path.read_text(encoding="utf-8"))
    }


def check_report(report, addresses):
    if report.get("format") != 1:
        raise ValueError("Unsupported reccmp report format")
    if not addresses:
        raise ValueError("No FUNCTION claims found; refusing a vacuous success")
    rows = {int(row["address"], 16): row for row in report["data"]}
    failures = []
    for address in sorted(addresses):
        row = rows.get(address)
        if row is None:
            failures.append(f"0x{address:08x}: absent from report")
        elif not row.get("recomp") or row.get("stub") or row.get("matching") != 1.0:
            failures.append(f"0x{address:08x}: {row.get('name')} has accuracy {row.get('matching')}")
    return failures


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("report", type=Path)
    args = parser.parse_args()
    try:
        report = json.loads(args.report.read_text(encoding="utf-8"))
        addresses = claimed_addresses(ROOT / "LEGO Racers 2")
        failures = check_report(report, addresses)
    except (KeyError, OSError, ValueError) as error:
        parser.exit(1, f"Cannot verify matches: {error}\n")
    if failures:
        parser.exit(1, "\n".join(failures) + "\n")
    print(f"Verified {len(addresses)} FUNCTION claims at 100% instruction accuracy.")


if __name__ == "__main__":
    main()
