"""Verify complete, relocation-free folded bodies at linker-map symbols."""

import csv
import hashlib
import json
from pathlib import Path
import re

from inspect_binary import pe_headers, rva_offset, u32
from setup import ORIGINAL_SHA256


def code_bytes(binary, address, size):
    _, optional, sections = pe_headers(binary)
    base = u32(binary, optional + 28)
    offset = rva_offset(sections, address - base)
    end = rva_offset(sections, address - base + size - 1) + 1
    if end - offset != size:
        raise ValueError("Function spans noncontiguous PE file data")
    return binary[offset:end]


def verified_aliases(original, rebuilt, link_map, aliases, sizes):
    """Fail closed: no relocation masking, prefix matches or missing symbols."""
    if hashlib.sha256(original).hexdigest() != ORIGINAL_SHA256:
        raise ValueError("Unsupported original executable SHA-256")
    symbols = {}
    wanted = {alias["symbol"] for alias in aliases}
    for line in link_map.splitlines():
        match = re.match(r"\s+[0-9a-fA-F]{4}:[0-9a-fA-F]+\s+(\S+)\s+([0-9a-fA-F]{8})\b", line)
        if match:
            name, address = match.groups()
            if name not in wanted:
                continue
            value = int(address, 16)
            if name in symbols and symbols[name] != value:
                raise ValueError(f"Ambiguous linker-map symbol: {name}")
            symbols[name] = value
    result = []
    seen = set()
    for alias in aliases:
        address = int(alias["address"], 16)
        expected = bytes.fromhex(alias["bytes"])
        if address in seen or not expected or sizes.get(address) != len(expected):
            raise ValueError(f"Invalid complete folded body extent: 0x{address:08x}")
        seen.add(address)
        recomp = symbols[alias["symbol"]]
        if code_bytes(original, address, len(expected)) != expected:
            raise ValueError(f"Retail folded body differs: 0x{address:08x}")
        if code_bytes(rebuilt, recomp, len(expected)) != expected:
            raise ValueError(f"Rebuilt folded body differs: {alias['symbol']}")
        result.append({"address": hex(address), "recomp": hex(recomp), "name": alias["name"],
                       "matching": 1.0, "type": 1, "folded": True})
    return result


def supplement_report(root, build_dir, inventory=None):
    original = (root.parent / "LEGO Racers 2" / "LEGO Racers 2.exe").read_bytes()
    rebuilt = (build_dir / "LEGO Racers 2.exe").read_bytes()
    link_map = (build_dir / "legoracers2.map").read_text(encoding="latin-1")
    with (root / "reccmp/folded-functions.csv").open(newline="") as stream:
        aliases = list(csv.DictReader(stream))
    with (root / "reccmp/function-sizes.csv").open(newline="") as stream:
        sizes = {int(row["address"], 16): int(row["size"]) for row in csv.DictReader(stream)}
    verified = verified_aliases(original, rebuilt, link_map, aliases, sizes)
    path = build_dir / "reports/LEGORACERS2.json"
    report = json.loads(path.read_text())
    rows = {int(row["address"], 16): row for row in report["data"]}
    for alias in verified:
        address = int(alias["address"], 16)
        if address in rows and (rows[address]["matching"] != 1.0 or rows[address].get("stub")):
            raise ValueError(f"Folded inventory conflicts with measured claim: 0x{address:08x}")
        rows[address] = alias
    report["data"] = [rows[address] for address in sorted(rows)]
    if inventory is not None:
        inventory.require_addresses(int(row["address"], 16) for row in report["data"]
                                    if row.get("type", 1) == 1)
        if report["function_count"] != inventory.total:
            raise ValueError("Report denominator differs from the original function inventory")
        report["function_inventory"] = inventory.metadata
    path.write_text(json.dumps(report, separators=(",", ":")))
    print(f"Verified {len(verified)} complete folded bodies against both PEs and exact linker-map symbols.", flush=True)
    # Re-render the same report so its additional measured rows are visible.
    from reccmp.compare.report import deserialize_reccmp_report, report_function_accuracy
    from reccmp.utils import gen_svg, write_html_report
    status = deserialize_reccmp_report(path.read_text())
    write_html_report(str(path.with_suffix(".html")), status, None)
    implemented, _, accuracy = report_function_accuracy(status)
    gen_svg(str(path.with_suffix(".svg")), status.filename, None,
            implemented, status.function_count, accuracy)
    return status
