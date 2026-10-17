#!/usr/bin/env python3
"""Validate implemented FUNCTION claims while preserving raw match scores."""

import argparse
import json
import math
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
NEAR_MATCH_THRESHOLD = 0.95
MARKER = re.compile(r"^\s*//\s*FUNCTION:\s+LEGORACERS2\s+(0x[0-9a-fA-F]+)\b", re.MULTILINE)


def claimed_addresses(source_root):
    return {
        int(match, 16)
        for path in source_root.rglob("*")
        if path.suffix in (".cpp", ".h")
        for match in MARKER.findall(path.read_text(encoding="utf-8"))
    }


def check_report(report, addresses, *, require_exact=False):
    if report.get("format") != 1:
        raise ValueError("Unsupported reccmp report format")
    if not addresses:
        raise ValueError("No FUNCTION claims found; refusing a vacuous success")
    rows = {int(row["address"], 16): row for row in report["data"]}
    failures = []
    minimum = 1.0 if require_exact else NEAR_MATCH_THRESHOLD
    for address in sorted(addresses):
        row = rows.get(address)
        if row is None:
            failures.append(f"0x{address:08x}: absent from report")
        else:
            accuracy = row.get("matching")
            if (not row.get("recomp") or row.get("stub")
                    or type(accuracy) not in (int, float)
                    or not math.isfinite(accuracy) or not minimum <= accuracy <= 1.0):
                failures.append(f"0x{address:08x}: {row.get('name')} has accuracy {accuracy}")
    return failures


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("report", type=Path)
    parser.add_argument("--require-exact", action="store_true", help="Require 100%% raw instruction matches")
    args = parser.parse_args()
    try:
        report = json.loads(args.report.read_text(encoding="utf-8"))
        addresses = claimed_addresses(ROOT / "LEGO Racers 2")
        failures = check_report(report, addresses, require_exact=args.require_exact)
    except (KeyError, OSError, ValueError) as error:
        parser.exit(1, f"Cannot verify matches: {error}\n")
    if failures:
        parser.exit(1, "\n".join(failures) + "\n")
    rows = {int(row["address"], 16): row for row in report["data"]}
    exact = sum(rows[address]["matching"] == 1.0 for address in addresses)
    print(f"Verified {len(addresses)} implemented FUNCTION claims: {exact} exact, "
          f"{len(addresses) - exact} reviewed near matches (at least {NEAR_MATCH_THRESHOLD:.0%}).")


if __name__ == "__main__":
    main()
