# Local reverse-engineering workspace

`tools/ghidra/ExportWorkRanges.java` exports an automatically discovered function
inventory and unreviewed C decompiler output for the initial parser, string and
math work ranges. It was exercised with the installed Ghidra 12.1.2 headless
analyzer. All generated output and the Ghidra project live in ignored `data/`.

From this repository, the initial import can be reproduced with:

```sh
mkdir -p data/ghidra-project data/ghidra-home data/ghidra-cache data/ghidra-export
JAVA_TOOL_OPTIONS="-Duser.home=$PWD/data/ghidra-home -Dapplication.cachedir=$PWD/data/ghidra-cache" \
  /opt/ghidra/support/analyzeHeadless data/ghidra-project LR2 \
  -import "../LEGO Racers 2/LEGO Racers 2.exe" \
  -scriptPath tools/ghidra \
  -postScript ExportWorkRanges.java "$PWD/data/ghidra-export" \
  -analysisTimeoutPerFile 300 -max-cpu 2 \
  -log data/ghidra-analysis.log -scriptlog data/ghidra-script.log
```

For an already imported project, replace `-import <path>` with
`-process "LEGO Racers 2.exe" -noanalysis`. Adjust the Ghidra installation path
if needed. The script saves candidate bodies as files named by original VA,
such as `data/ghidra-export/0045dcb0.c`.

Additional ranges can follow the export directory as hexadecimal `start:end`
arguments. Starts are inclusive and ends exclusive. For example, exporting
the file and vector helper range from the existing project uses:

```sh
JAVA_TOOL_OPTIONS="-Duser.home=$PWD/data/ghidra-home -Dapplication.cachedir=$PWD/data/ghidra-cache" \
  /opt/ghidra/support/analyzeHeadless data/ghidra-project LR2 \
  -process "LEGO Racers 2.exe" -noanalysis -scriptPath tools/ghidra \
  -postScript ExportWorkRanges.java "$PWD/data/ghidra-export" 004a4300:004a5300 \
  -max-cpu 2 -log data/ghidra-analysis.log -scriptlog data/ghidra-script.log
```

Omitting ranges selects the three initial worker ranges. The inventory always
covers the entire program; only the decompiled body exports are range-filtered.

The initial analysis discovered **4,228 functions** and exported 100 bodies in
the three worker ranges. This is an analysis result, not a verified game-code
function count: it includes runtime code and thunks and can miss functions.
Neither its count nor its body sizes should be used directly as the project's
progress denominator or the reccmp size CSV. Check full control-flow paths and
callers against disassembly before implementing a candidate.

When parallel work is authorized, give each worker separate source ownership
and its own build directory. A worker manifest contains repository-relative
source paths and can be used with:

```sh
python tools/build.py --source-list build-worker/sources.txt --build-dir build-worker
```

The integration owner merges `sources.txt` and verified function-size entries,
then rebuilds and checks every `FUNCTION` claim together. A worker reporting
100% effective similarity alone does not meet the instruction-match requirement.

## Automatic function inventory

The current Ghidra inventory in
`data/ghidra-export/discovered-functions.csv` contains **4,254 distinct
function starts**, including **21 entries marked as thunks** and linked
CRT/library code. All 4,254 records are internal to the executable rather
than external imports. This is the automated discovery snapshot, not a
verified count of game-authored functions. Analysis can miss functions or
split/merge boundaries; manually reviewed callbacks already demonstrate
that the inventory is incomplete. It must not be used as a whole-game
completion denominator. Source reconstruction and instruction matches are
listed separately in [progress](progress.md).
