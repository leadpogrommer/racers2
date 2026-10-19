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

The latest registry/renderer-cleanup checkpoint was verified on 2026-10-18 in both the Python
builder and CMake/NMake: **253 exact functions, ten reviewed near matches,
and 316 STUB rows**. All 579 measured raw scores and annotation statuses agree.
Both outputs pass data comparison for 118 annotated globals, application wiring
for 76 pointers, renderer resource/buffer wiring for 78 pointers, input wiring for 80 pointers, controller/clock wiring for 12 pointers, and all 307 input-format entries.
The 16 tooling tests pass; regressions cover optional D3DX8 configuration and ANSI/non-UTF-8
archive member names in VC6 link maps, preserving the ASCII symbol/address
columns used by the static verifiers. No game behavior tests were added.

The compiled initializer at `0x004ac130` is still STUB at 22.87%; its presence
does not establish a runnable renderer. No game or probe was executed for this
checkpoint. The annotated abstract lookup table uses the real CRT purecall,
whose nine-byte archive identity was reviewed separately from game functions.

### Earlier checkpoints

Before this extended renderer batch, both builds were revalidated on 2026-10-17.
Both passed the match guard for **154 exact functions and four reviewed near
matches**. All 266 measured raw scores and annotation statuses agree across
the two builds: 158 implemented reconstructions and 108 unfinished bodies.
The CLI denominator of 269 also includes the unmatched 10-byte generated
cleanup at 0x004a2550, which has no unique name mapping, and the one-byte
save-config exit callback at 0x0049b930, which folds with the heap callback.
The null-return helper at 0x00473140 also folds with the existing zero leaf.
These three inventory-only functions do not add measured match rows.
Annotation lint and formatting checks passed. The existing 13 tooling tests
last passed with the preceding input batch; they cover the match guard and
library identification. The new input-table
verifier checks all 80 original targets and rebuilt pointers in each build;
the existing application verifier still passes all 76 pointers. No game
behavioral tests were added.

Data comparison verified 65 annotated globals with zero issues in both
builds. New storage includes the pool lists/arena/guard/probe state, file
initialization and redirection flags, compressed-block cache, error-dialog result, saved
config pointers/guard/record, main/view handles, title buffer, accelerators and
the mouse-axis scale (verified as bits `0x3c808081`). The latest input batch
also verifies the static service and selector, joystick HRESULT/last-device
pointer, shared force-feedback offsets, and six effect-description workspaces
and their first-use flags.
The renderer follow-up adds 13 measured bodies: four exact and nine below
threshold. Its new clear-result HRESULT at `0x0067bd58` is separately mapped
from the existing renderer initialization HRESULT. Both builders agree on every
measured score/status; all 65 annotated globals compare with zero issues.
The application/input pointer and input-format checkers pass for both outputs.
Only compilation, instruction comparison and static data checks were run for
this follow-up; the existing 13 tooling tests were unchanged and were not
repeated. No game behavior tests or game/probe execution were added.
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

These results cover the recovered subset. The automatic 4,254-start inventory
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
The normal guard passes all 242 implemented functions. Strict exact mode
rejects the nine reviewed near matches, preserving their raw scores. These
include the bitmap storage cleanup at 95.24%, stage-priority masking at 96.97%,
and high-resolution texture policy at 98.11%, alongside the four previously
reviewed functions. The 16 tooling tests cover the near-match boundary,
strict mode, invalid scores, missing functions, library identification and
legacy link-map decoding.


## Renderer resource checkpoint

Both builders also verify all 20 texture, animation and abstract callback
pointers with `tools/check_renderer_resource_dispatch.py`. The genuine static
D3DX8 surface-copy routines were identified by COFF relocation-masked byte
comparison and linked from the pinned external SDK; see [evidence](work-d3dx8.md).
The 76 checked globals include the reserved renderer storage and selector,
three timing globals, and texture transform matrices. The renderer is still
unconstructed and the temporary entry point is unchanged. No game or asset
probe was run for this batch.


## Renderer submission and loading checkpoint

That checkpoint verified 500 measured rows: 233 exact matches, nine reviewed
near matches and 258 STUBs, plus 82 annotated globals without data issues.
`check_renderer_resource_dispatch.py` now verifies 78 callbacks across texture,
cache, abstract, vertex/index-buffer and animation-resource tables. Application
and input dispatch (76 and 80 pointers) and all 307 input-format entries still
pass. Sixteen tooling tests pass. No game or asset probe was executed.

The complete renderer table and actual singleton construction remain staged
unlinked work. The temporary entry point still does not start the game. Large
loading/maintenance functions remain STUB after review found and corrected
incorrect mip offsets, CRT case conversion and surface-copy contracts; their
compilation alone is not evidence of correct rendered pixels.

### Scene, GUI and controller checkpoint

Both builders agree on 533 measured function rows: 245 exact, nine reviewed near matches and 279 STUB. The twelve added exact matches are controller cleanup, device assignment, binding append/lookup, auxiliary-renderer lookup, and seven timer/mutex wrappers. Device reset, resource restoration, scene startup, packet drawing, clipping, tagged quads and bump vectors retain their measured STUB status. Both data reports verify 95 globals with zero issues. All 78 renderer resource/buffer callbacks, 76 application callbacks, 80 input callbacks and 307 input-format entries pass static checks. Sixteen tooling tests pass. No game or asset probe ran.

Identical-code folding now selects different public aliases for existing empty callbacks and path setters. FOLDED annotations retain one original comparison address through the actual shared body, without adding writes or disabling linker folding. The full 207-slot renderer table verifier is prepared but cannot be run successfully until those tables are linked.

## Controller and clock checkpoint (2026-10-18)

Both builds agree on 560 measured rows: 252 exact, ten reviewed near matches, and 298 STUB. The three exact clock methods are the zero-value callback, calibration getter and elapsed-microseconds update. Four new exact controller methods are current-frame calculation, mutex release, polling initialization and the timer callback. Controller shutdown is reviewed at 98.18%: its only instruction difference is clearing the unused EDX bridge argument before the stop callback. Both data reports verify 99 globals with zero issues, and both complete controller/clock tables pass pointer verification. The readiness helper now connects renderer resource initialization, DirectInput and controller timing in the original order. The temporary entry point does not invoke it. No game run or incremental startup probe was performed.

## Registry, scheduler and renderer cleanup checkpoint (2026-10-18)

Both builders agree on 579 measured rows: 253 exact, ten reviewed near matches and 316 STUB. The game-manager shutdown wrapper adds one exact match. The fixed-step scheduler, registry classification/update/removal loops, terrain/material draft and renderer resource shutdown have compiled source with their raw scores retained. The terrain implementation remains incomplete for grouped batches, topology and software processing. Both data reports verify 118 globals with zero issues; registry initialization now uses the retail 32-pointer backing array, capacity32 and nonzero initial flags. All previously verified callback tables and 307 input-format entries pass in both outputs. No game, asset or startup probe was run. Full renderer-table linkage, default audio initialization and concrete menu-object creation remain open.


## Renderer construction and registry initialization checkpoint (2026-10-18)

Both builders agree on 595 measured rows: 253 exact, ten reviewed near matches and 332 STUB. The complete constructor, lookup drawing/flush paths, object-registry allocation, insertion, removal and initialization are linked for measurement. Both data reports verify 122 annotated globals with zero issues. The 103-slot base and derived renderer tables plus three two-slot lookup tables pass all 212 pointer checks. Lookup tables use explicit ABI bridges, verified against the retail targets and rebuilt link map rather than compared as pointers to the underlying member methods. All existing application, input, resource/buffer, controller/clock and input-format checks pass in both builds.

The constructor is available for startup integration; it is not yet invoked by the temporary entry point. Audio, frame rendering, concrete game objects and the real WinMain are still being recovered in separate unlinked units. Their missing-symbol notices do not represent measured functions or verified behavior. No game, startup or asset probe ran.


## Frame support checkpoint (2026-10-18)

Both builders agree on 604 measured rows: 254 exact, ten reviewed near matches and 340 STUB. Quaternion shortest-path interpolation adds one exact match. Component transform interpolation, hue-color generation, controller pause/origin adjustment, frame preparation and the configuration-disabled profiler paths compile with their raw scores preserved. Both data reports verify 122 globals with zero issues; all 212 renderer/lookup callbacks, 78 resource/buffer callbacks, 76 application callbacks, 80 input callbacks, twelve controller/clock callbacks and 307 input-format entries pass in both outputs. Sixteen tooling/library-identification tests pass.

WinMain, message handling, teardown, frame orchestration, global pause and collision transform/movement sources also passed object-only VC6 compilation. They remain unlinked until their actual service, dispatch and object dependencies exist. They are excluded from the 604 measured rows. No game or incremental startup/asset probe ran. See [frame and startup recovery](work-game-frame.md).


## Spatial storage and grid checkpoint (2026-10-19)

Both builders agree on 647 measured rows: 269 exact, ten reviewed near matches and 368 STUB. Slot storage, linked registries, static grid insertion/removal, axis sorting and dynamic-index construction now link. All eight spatial dispatch pointers pass verification against the retail tables and rebuilt map. The generic sorting helpers retain low raw scores and remain STUB; no instruction-match claim is made for them. Both data reports verify 123 globals with zero issues. Sixteen tooling/library-identification tests pass.

The audio-mode selector is linked, while the full audio singleton and startup/frame dependencies remain staged. No game, startup or asset probe was run.


## Lifecycle, parser and capture checkpoint (2026-10-19)

Both builders agree on 661 measured rows: 271 exact, ten reviewed near matches and 380 STUB. The physical world-matrix rebuild and angular-limit square initializer add two exact matches. Parent detachment, parent-point velocity, velocity clamps, audio format parsing/conversion and screenshot capture retain their raw measurements and STUB status. Both data reports verify 127 globals with zero issues. The Wine DX8 bridge now keeps the genuine USER32 GetSystemMetrics declaration after the VC6 multimon header.

No game, screenshot, startup or asset probe was run. The temporary entry point remains until the complete frame/audio/object path links.


## Contact impulse and frame-state checkpoint

Both SP5 builders pass with 670 measured rows: 272 exact functions, ten reviewed near matches and 388 STUB reconstructions. Their raw scores and statuses agree. The mass-weighted contact separation helper adds an exact match; the other impulse bodies retain their measurements and unresolved floating-point/aliasing status. Data comparison verifies 136 annotated globals with zero issues. The spatial-list dispatch checks pass in both builds. No game or runtime probe ran.


## Commentary decoder and folded-function checkpoint

Both SP5 builders pass with 699 measured rows: 277 exact functions, eleven reviewed near matches and 411 STUB reconstructions. All raw scores/statuses agree. Data comparison verifies 143 annotated globals with zero issues. Renderer resource/buffer dispatch verification passes all 78 pointers in both builds. The tooling/library suite passes 21 tests, including five fail-closed folded-body regressions. The byte-verification supplement independently checks three complete functions against both PEs and exact linker-map symbols before filling reccmp's folded aliases. Reports include those rows in JSON, HTML and SVG.

Official Ogg/Vorbis source replaces the need to reconstruct the linked codec. Twenty-five public library interfaces have source/caller evidence; 94 relocation-masked internal candidates remain separately inventoried. Commentary and request wrappers link, but stream staging ownership, larger low-score bodies and service/provider table integration remain unresolved. No game, audio playback, asset or runtime probe ran. See [decoder identification and limits](work-xiph-decoder.md).

## Contact solver and constraint checkpoint

Both SP5 builders pass with 722 measured rows: 281 exact functions, twelve reviewed near matches and 429 STUB reconstructions. All measured scores and annotation statuses agree. Data comparison verifies 147 annotated globals with zero issues. The six cached-constraint dispatch pointers and eight spatial callbacks match the retail targets in both builds. Four new difference/matrix helpers match exactly; the world-position accessor is reviewed equivalent at 98.70%. The solver, force application and contact response remain under review. No game or runtime probe was run.

## Frontend preferences and localization checkpoint

Both SP5 builders pass with 742 measured rows: 289 exact functions, twelve reviewed near matches and 441 STUB reconstructions. All scores/statuses agree. Data comparison verifies 148 annotated globals with zero issues. Eight added helpers match exactly, including the localization constructor, string access/path builder, narrow name copying and four other record initializers. Four genuine folded bodies pass complete-byte and linker-symbol verification in both builds. The larger loader, language relocation, input binding setup and preferences retain STUB status. GameController storage/construction and its complete 21-slot dispatch pass object compilation and remain staged. No game or runtime probe was run. See [frontend recovery](work-game-controller.md).

## Frame, audio, font and collision integration

Both SP5 builders link 247 translation units. The matching reports contain
1,061 measured functions: 299 exact, twelve reviewed near matches and 750 STUB
reconstructions. All raw scores/statuses agree between the builders. The ten
newly promoted exact helpers have complete raw extents and reviewed caller/data
contracts. Four genuine folded bodies still pass full-byte/linker-map checks.

Data comparison verifies 201 annotated globals with zero issues. The complete
audio service/provider tables have 144 original targets and 144 emitted callback
pointers checked independently, alongside the existing renderer, application,
input, spatial-list and constraint table checks. The 21 tooling/library tests
pass. No game-function behavioral tests or runtime/game probes were run.

GameController initialization and concrete frontend transition sources remain
staged; the temporary bootstrap is still the executable entry point. See
[integration notes](work-main-integration.md).


## Frontend resource and shared menu checkpoint (2026-10-19)

Both Python/SP5 and CMake/NMake builds pass with 275 translation units.
Their reports agree on 1,149 measured functions: 341 exact, sixteen reviewed
near matches, and 792 STUB reconstructions. All 357 FUNCTION claims pass.
Six genuine folded aliases have complete original/emitted-byte and exact
linker-map checks. Both data reports verify 217 globals with zero issues.
All nine dispatch inventories pass in both builds, including the 44 newly
checked bodypart/glyph/ScreenPrinter pointers and 144 audio pointers.
The 21 existing tooling/library-identification tests pass.

The batch adds controller defaults/player cars/session setup, bodypart lists,
menu text/icon ownership, HUD/pause/builder image loading, sprite geometry,
font metrics and the shared RNG. It also removes duplicate storage for the
controller's network/reverb pointers. The DirectPlay SDK check compiles and
links against genuine SDK interfaces and was not executed. See
[frontend notes](work-controller-frontend.md).

No game, menu, startup, movie or asset probe was executed. The temporary
bootstrap remains; open widget, scene-manager, network and startup-movie
closures still prevent the intended menu milestone.

## Menu panels and geometry

Both builders pass with 280 translation units. All 1,177 measured scores and
annotation statuses agree: 349 exact FUNCTION claims, sixteen reviewed near
matches, and 812 STUB reconstructions. All 365 claims and the six complete
folded aliases pass. Both data reports verify 218 globals with zero issues;
all nine dispatch inventories still pass in each build. No runtime test or
game behavioral test was added. See [panel notes](work-menu-panels.md).

## Menu widget integration

Both 287-unit builds pass. All 1,239 measured scores and statuses agree: 374
exact FUNCTION claims, sixteen reviewed near matches, and 849 STUB bodies.
All 390 claims and eight complete folded bodies pass. Both data reports verify
228 globals with zero issues. All ten dispatch inventories pass, including
48 newly checked widget callbacks. The existing 21 tooling/library tests pass.
The Python response-file link also verifies the isolated long-path build.
No runtime or game behavioral test was added. See [widget notes](work-menu-widgets.md).

## Controller service and scene callbacks

Both 294-unit builds pass and agree on 1,350 measured raw scores: 397 exact
functions, eighteen reviewed near matches and 935 STUB reconstructions. Both
data reports verify 242 globals with zero issues. All eleven dispatch checks,
eight complete folded aliases and the existing 21 tooling/library tests pass.
No runtime probe was executed. See [batch evidence](work-controller-recording.md).

## Track, network status and character preview callbacks

Both 313-unit builds agree on 1,441 measured raw scores: 434 exact FUNCTION
claims, nineteen reviewed near matches and 988 STUB bodies. Data comparison
verifies 252 globals with zero issues in each build. All eleven dispatch
inventories pass, including eleven widget tables (66 slots), along with nine
complete folded aliases and the existing 21 tooling/library tests.
No runtime probe was executed. See [batch evidence](work-frontend-preview.md).

## Character preview initialization and foyer event updates

Both 318-unit builds agree on all 1,461 measured raw scores and statuses:
442 exact FUNCTION claims, nineteen reviewed near matches and 1,000 STUB
bodies. All 461 claims and nine complete folded aliases pass. Both data reports
verify 254 globals with zero issues; all eleven dispatch inventories and the
existing 21 tooling/library tests pass. No game or incremental runtime probe
was executed. See [batch evidence](work-foyer-events.md).

## Startup movie integration

Both 325-unit builds pass and agree on all 1,475 measured scores/statuses:
445 exact FUNCTION claims, nineteen reviewed near matches and 1,011 STUB
bodies. All 464 claims and eleven complete folded aliases pass. Both data
reports verify 256 globals with zero issues. All twelve static dispatch
checkers pass, including the three movie-service slots and three game-owned
DirectShow overrides. SDK inherited callbacks remain outside that new check.
No runtime or game behavioral test was run. See [movie evidence](work-startup-movies.md).


## Input controls and model dependencies (2026-10-19)

The canonical Python and CMake/NMake SP5 builds compile and link 338 units.
Both reports agree on 1,540 measured rows: 467 exact functions, nineteen
reviewed near matches and 1,054 STUB rows. All twelve dispatch verifiers pass
in both PEs, including all 78 callbacks in thirteen menu-widget tables. Data
comparison has zero issues across 263 globals. The 21 retained tooling/library
tests pass. No game behavior tests or runtime/asset probes were added or run.
See [recovery evidence and remaining limits](work-menu-input-and-model-resources.md).


## Model geometry, collision and shadows (2026-10-19)

Both canonical SP5 builders compile and link 344 units and agree on all 1,564
measured scores/statuses: 472 exact functions, nineteen reviewed near matches
and 1,073 STUB rows. All twelve dispatch verifiers pass in both PEs, alongside
eleven complete folded aliases. Data comparison reports zero issues across
263 globals. All 24 new function extents were reviewed. No game behavior tests
or runtime/asset probes were added or run. See [model evidence](work-model-loading.md).


## Loading screen and fade controller (2026-10-19)

Both 348-unit SP5 builds pass and agree on all 1,591 measured scores/statuses:
481 exact functions, nineteen reviewed near matches and 1,091 STUB rows. All
twelve dispatch verifiers pass in both PEs, with the scene check expanded to
146 pointers. Twelve complete folded aliases and 264 data globals with zero
issues pass. The 21 existing tooling/library tests pass. No game behavior tests
or runtime probes were added or run. See [loading-screen evidence](work-loading-progress.md).


## Model draw queues (2026-10-19)

Both 350-unit SP5 builds pass and agree on all 1,595 measured scores/statuses:
482 exact functions, nineteen reviewed near matches and 1,094 STUB rows. All
twelve dispatch checks and twelve complete folded aliases pass in both PEs.
Data comparison reports zero issues across 265 globals. The frontend widget
factory compiles separately but awaits car-preview linking. No runtime probe
was run. See [model-drawing evidence](work-model-drawing.md).


## Shadow generation and menu configuration (2026-10-19)

Both canonical SP5 builders compile and link 353 units and agree on all 1,601
measured scores/statuses: 482 exact functions, nineteen reviewed near matches
and 1,100 STUB rows. All twelve dispatch inventories and twelve complete folded
aliases pass in both PEs. Data comparison reports zero issues across 267 globals,
including both recovered input-control tables. All six new function extents were
reviewed against the original code. No tooling logic changed; the 21 retained
tooling tests passed in the preceding loading-screen batch. No game behavior
tests or runtime probes were added or run. See [batch evidence](work-shadow-and-configuration.md).


## Menu backdrop and startup supervisor (2026-10-19)

Both canonical SP5 builders link 356 units and agree on all 1,616 measured
scores/statuses: 487 exact functions, nineteen reviewed near matches and 1,110
STUB rows. All twelve dispatch checkers pass in both builds, with scene inventory
expanded to 167 pointers. Twelve folded aliases and 267 annotated globals with
zero data issues pass. The backdrop fade constant also matches retail bytes
through an independent map/PE check. All 21 existing tooling tests pass. No game
behavior tests or runtime probes were added or run. See [batch evidence](work-menu-background.md).


## Car preview and frontend resources (2026-10-19)

Both canonical SP5 builders link 363 units and agree on all 1,701 measured
scores/statuses: 504 exact functions, twenty-one reviewed near matches and
1,176 STUB rows. All twelve dispatch checkers pass in both builds; scene/model
inventory now covers 314 pointers and menu widgets cover 84 pointers. Twelve
complete folded aliases pass. Data comparison has zero issues across 272
globals; the model-binding depth float is separately checked through both
linker maps and complete original bytes. All 21 retained tooling tests pass.
No game behavior tests, asset probes or game runs were performed. See
[car-preview evidence](work-car-preview.md).
