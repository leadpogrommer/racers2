# Initial validation

Validated on Linux with Wine, Python 3.12.15, the pinned reccmp revision and
MSVC SP5 toolchain described in [provenance.md](provenance.md).

Both `tools/build.py` and the Windows CMake 3.26.6 / NMake build were exercised
under Wine. Both produced readable PE/PDB pairs and these instruction matches:

| Original address | Function | Bytes | reccmp accuracy |
| --- | --- | --- | --- |
| `0x0045dda0` | `IsTokenCharacter` | 81 | 100% |
| `0x0045de00` | `IsHorizontalSpace` | 21 | 100% |

The compiler-produced `token.obj` carries `@comp.id = 0x000b2306`, matching
the dominant C++ Rich record in the original. The original Rich checksum and
SHA-256 were verified, and fresh inspection agrees with `original-binary.json`.

Annotation lint, clang-format checks on the new C++ files, and five tests for
the match guard passed. Those tests cover exact success, effective-only
similarity, missing/unlinked functions, stubs, empty claims and unsupported
report formats. The GitHub Actions commands were exercised locally; no remote
CI run or publication was performed.

The generated reports are ignored build artifacts:
`build/reports/LEGORACERS2.{html,json,svg}`. These two matches validate the
workflow; they do not measure whole-game coverage. The CRT startup remains a
library comparison, and the temporary entry point does not start the game.

In the supplied workspace, `.venv` and `.tools/msvc6-sp5` are already installed.
To rebuild and regenerate reports using its existing Wine prefix:

```sh
cd /home/ilya/work/lr2_decomp/racers2
source .venv/bin/activate
export WINEPREFIX=/home/ilya/work/lr2_decomp/wine_pfx
python tools/build.py
python tools/compare.py
```

## Continued decompilation

The combined SP5/Wine and CMake/NMake builds were revalidated on 2026-10-17.
Both passed the match guard for **150 exact functions and four reviewed near
matches**. All 253 measured raw scores and annotation statuses agree across
the two builds: 154 implemented reconstructions and 99 unfinished bodies.
The CLI denominator of 256 also includes the unmatched 10-byte generated
cleanup at 0x004a2550, which has no unique name mapping, and the one-byte
save-config exit callback at 0x0049b930, which folds with the heap callback.
The null-return helper at 0x00473140 also folds with the existing zero leaf.
These three inventory-only functions do not add measured match rows.
Annotation lint, formatting checks and all 13 tooling tests passed. Those
tests cover the match guard and library identification. The new input-table
verifier checks all 80 original targets and rebuilt pointers in each build;
the existing application verifier still passes all 76 pointers. No game
behavioral tests were added.

Data comparison verified 64 annotated globals with zero issues in both
builds. New storage includes the pool lists/arena/guard/probe state, file
initialization and redirection flags, compressed-block cache, error-dialog result, saved
config pointers/guard/record, main/view handles, title buffer, accelerators and
the mouse-axis scale (verified as bits `0x3c808081`). The latest input batch
also verifies the static service and selector, joystick HRESULT/last-device
pointer, shared force-feedback offsets, and six effect-description workspaces
and their first-use flags.
Both builds completed without compiler warnings. The pool and archive reader
are now in the main build as required startup dependencies, with unresolved
provenance kept explicit. [Current progress](progress.md) lists the scores.

An independent control-flow review of the 524-byte decoder found no semantic
mismatch, including overlap, header skip, control-bit refills and count escapes.
Its instruction score remains low and its annotation stays STUB. Review of the
archive initializer caught and corrected premature closure of the persistent
GTC handle. The shipped metadata has exactly 5,767 file records of 0x88 bytes
and 2,187 compression records of 0x14 bytes; physical chunks are contiguous
and end exactly at the 135,813,120-byte archive size.

A bounded runtime check launched the rebuilt executable from the supplied
installation under Wine and exited with code 0. Startup constructed the
application and accessed the per-user
`C:\users\ilya\Documents\Games\LEGO Racers 2\Saved Games\LR2.cfg` path,
then read all 784,312 bytes of FILELIST.INF and 43,740 bytes of COMPRESS.INF
and opened GAMEDATA.GTC read-only. File tracing is retained in ignored
`build/runtime-application-startup.log`. This exercises application config
startup, pool allocation and archive metadata initialization. Window creation, rendering and
gameplay were not exercised by this startup run. A separate asset probe is
described below. The entry point remains temporary. See
[runtime support](work-runtime-support.md) and
[application runtime](work-application-runtime.md).

The repeatable DX8 compile/link probe also passed through `tools/build.py`
using its generated JSON configuration, pinned VC6 tools and local Wine
header bridge/import archives. The full Python and CMake builds also passed
with that generated configuration. PE imports in the probe include
Direct3DCreate8, DirectInput8Create and DirectSoundCreate8. The probe was not
executed; see
[DirectX prerequisites](work-directx8.md).

Both builders include the resource file when it exists under
`build-resources/retail.res`; its five entries (7,022 payload bytes) were
round-trip checked against the pinned executable and confirmed in both output
PEs with identical payloads. The complete application dispatch-table binary
verifier passes 76/76 callback pointers in both PE
outputs. That is a wiring check and does not assert exact method bodies or
semantic equivalence. Display config, native-window helpers and renderer
adapter work compile, but startup has not created a window or rendered a
frame. Both builders import the real SHFOLDER and GDI32 APIs. See
[configuration recovery](work-application-config.md),
[resource extraction](work-resources.md) and
[application runtime](work-application-runtime.md).

An ignored `build-asset-probe` executable exercised the real `WinFile` and
`MemoryHeap` path against three installed archive entries and exited with code
0. It read `HEADS.TXT` (646 bytes, SHA-256
`0112c0e03babbb4bd14ecc90f6810c0130a3f5d16c100012b52381333de7f47a`),
`PA DIAL.MIP` (349,562 bytes across three logical 128 KiB blocks, starting at
archive offset 3,438,592), and `SCRIPT KEY VALUE PAIRS.TXT` (6,977 bytes at
logical offset 272,885,760, SHA-256
`10becebd8ad77f23e2d97791b45210f2e07bbf97ade81b0330c75187c59aeb0e`). The
small text files were 100% printable with 55 and 255 CRLF lines respectively.
This verifies selected real compressed reads, including a multiblock texture
and a late archive entry. It does not establish pixel correctness or compare
the texture against an independently decoded retail-byte oracle, and it does
not create a window or render a frame.

The subsequent renderer and DirectInput helper batch was validated by
compilation, reccmp and static table checks only. The latest input batch adds
14 measured bodies: seven exact, one reviewed near match and six below the
annotation threshold. It also emits the mouse and keyboard constructors and
complete dispatch tables. The subsequent joystick/service batch adds 58
measured bodies: 33 exact and 25 below the annotation threshold, plus the
inventory-only folded null helper. The full service initialization source now
includes joystick axes and effects, with startup integration still pending.
Both builds agree on every score and annotation. No game or probe was executed
during these batches.
The new `tools/check_input_formats.py` verifies the input format headers and
all 307 object entries in both builds. Linked SDK keyboard and joystick data
match retail directly; the mouse format preserves retail's null button GUID
filters, which differ from Wine's current SDK data. The existing application
checker still verifies 76 callback pointers in each build.

The next runtime execution is reserved for a significant milestone expected
to reach a rendered menu or gameplay; the historical runtime evidence above is
unchanged.

These results cover the recovered subset. The automatic 4,228-start inventory
includes libraries and thunks and is not a whole-game completion denominator.
The executable still uses a temporary entry point and has not reached rendered
gameplay.

## Functional recovery policy

The primary objective is a playable rebuilt game. A reviewed function with
identical behavior and a near-perfect raw instruction match (initially at
least 95%) uses FUNCTION instead of STUB. The normal guard verifies those
implemented claims without replacing their real scores. Exact and reviewed
near matches are reported separately. The optional `--require-exact` mode
continues to reject every sub-100% raw match, including effective-only matches.
The normal guard passes all 154 implemented functions. Strict exact mode
continues to reject the four reviewed near matches: two at 95.24%, one at
95.33% and one at 95.83%. Their source annotations and work notes retain the
actual scores and equivalence review; no function was promoted solely because the policy
changed. The 13 tooling tests cover the near-match boundary, strict mode,
invalid scores, unresolved/missing functions and library-identification
fixtures.
