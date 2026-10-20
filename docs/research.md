# Original function inventory and research workspace

Reporting uses the tracked [`function-inventory.csv`](../reccmp/function-inventory.csv),
containing **6,612 unique internal executable entry addresses**: **5,647 bodies**
and **965 thunks**. The companion JSON binds the CSV checksum and count to the
supported original executable's SHA-256. Reports do not require a Ghidra project.

This is a machine-code entry inventory, including linked CRT/third-party libraries,
compiler helpers and exception cleanup functions. External DLL imports and inlined
functions do not have separate internal bodies and are excluded. Folded source
symbols sharing one original entry count once; distinct original entries that
share a rebuilt body still count separately. The executable is stripped, so this
is not a proven exhaustive source-function count or a game-authorship classification.
Discovery and boundary corrections can change it.

## Evidence for the expanded count

The previous report's 2,734 denominator counted annotated functions, not all
original functions. Even the previous 4,847-entry Ghidra snapshot missed callable
startup entries. The current address set is the deduplicated union of:

| Entry source | New starts |
| --- | ---: |
| Previous Ghidra discovery | 4,847 |
| Reviewed boundaries already in `function-sizes.csv` | 5 |
| CRT-called entries in `[0x0053d000, 0x0053dee8)` | 945 |
| Body targets of pure CRT entry jump wrappers | 806 |
| CRT-called entries in `[0x0053deec, 0x0053df00)` | 3 |
| Additional callbacks discovered after reanalysis | 6 |
| **Total** | **6,612** |

Original CRT instructions at `0x0051b180` and `0x0051b18a` pass those tables
to `_initterm` at `0x0051b27b`, which calls every nonzero pointer. The large
table has 953 unique targets. Of those, 942 are pure `E9 rel32` wrappers,
followed by eleven alignment NOPs; their unconditional targets are callable
bodies, not arbitrary branch labels. For example, slot `0x0053d004` points to
`0x00401000`, a jump to the initializer body at `0x00401010`.

The five reviewed missing entries were `0x0049b930`, `0x004a2550`, `0x004a2e10`,
`0x004c32b0` and CRT helper `0x0051a08f`. Reanalysis retained all prior entries
and found six legitimate indirect callbacks: `0x0048c540`, `0x0048c740`,
`0x004a1930`, `0x004a1980`, `0x004a1a10` and the `CreateThread` entry at
`0x004f1320`. The first five have original callback-table references; the last
is passed to `CreateThread` at `0x004f142d`.

All inventoried entries lie in executable `.text` at `[0x00401000, 0x0052ea08)`.
The PE has no export, TLS, exception or load-configuration directory supplying
another entry list. Its 163 external imports are not added to the total.
Ghidra bodies can be noncontiguous; a function's maximum-address span must not
be treated as a contiguous body or used to reject every nested entry.

## Refreshing discovery and reports

The Ghidra project, decompiler output and unfiltered discovery CSV remain ignored
under `data/`. The checked-in inventory is the reviewed reporting snapshot.
Ghidra 12.1.2 was used for this analysis. For an existing project:

```sh
JAVA_TOOL_OPTIONS="-Duser.home=$PWD/data/ghidra-home -Dapplication.cachedir=$PWD/data/ghidra-cache" \
  /opt/ghidra/support/analyzeHeadless data/ghidra-project LR2 \
  -process "LEGO Racers 2.exe" -scriptPath tools/ghidra \
  -preScript SeedFunctionInventory.java "$PWD/reccmp/function-sizes.csv" \
  -postScript ExportWorkRanges.java "$PWD/data/function-inventory-export" 00400000:00400001 \
  -analysisTimeoutPerFile 300 -max-cpu 2
python tools/function_inventory.py --refresh data/function-inventory-export/discovered-functions.csv
python tools/compare.py
```

For a fresh project, create `data/ghidra-project`, then replace `-process` with
`-import "../LEGO Racers 2/LEGO Racers 2.exe"`. Review the address-set diff before
accepting a refreshed snapshot; automatic discovery can miss or split/merge
functions. A new database may need the same manually reviewed entries.
`--original /path/to/executable` selects a non-default original for refresh.
`python tools/function_inventory.py` validates and prints the tracked count.
Refresh rejects unsupported originals, duplicate/non-executable entries and
inventories missing reviewed function-size entries. It also rejects removal of
existing inventory entries unless `--allow-removals` explicitly accepts reviewed
boundary corrections. Comparison rejects measured function entries missing from
the inventory rather than silently switching totals.

`SeedFunctionInventory.java` seeds the evidenced CRT entry/body pairs and reviewed
size-record starts before automatic analysis. It reports overlapping entries for
manual review rather than splitting existing bodies speculatively.
`ExportWorkRanges.java` always exports the entire function inventory; optional
`start:end` arguments restrict only decompiled body exports. The empty-code range
above requests inventory only. Omitting ranges exports the initial parser, string
and math ranges. To export a particular range without reanalysis:

```sh
JAVA_TOOL_OPTIONS="-Duser.home=$PWD/data/ghidra-home -Dapplication.cachedir=$PWD/data/ghidra-cache" \
  /opt/ghidra/support/analyzeHeadless data/ghidra-project LR2 \
  -process "LEGO Racers 2.exe" -noanalysis -scriptPath tools/ghidra \
  -postScript ExportWorkRanges.java "$PWD/data/ghidra-export" 004a4300:004a5300 \
  -max-cpu 2
```

Decompiler output and discovered body sizes remain unreviewed. Confirm complete
control flow, callers and boundaries before implementing functions or adding
verified sizes to `reccmp/function-sizes.csv`.

When parallel recovery is authorized, use separate ownership and build directories
under `build/workers/<worker>`. The integration owner merges manifests and size
records, rebuilds and checks all `FUNCTION` claims. Effective similarity alone
does not establish semantic equivalence; see [the contribution rules](../AGENTS.md).
