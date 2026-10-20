# Car preview and frontend resources

The canonical build now links the menu car selector, its display objects and
shared model bindings, plus frontend widget creation and menu resource ownership.
GameController/Secondary Manager startup and the foyer graph remain staged.
Bootstrap is still the entry point; this is not a rendered-menu milestone.

## Recovered behavior

The 0xa0-byte CarDisplay owns nine groups of four wheel/part objects and a
36-entry name-object array. Its resource owner is SecondaryManager+0x74.
Construction uses real model paths and templates; selection updates wheel
orientation/position and looks up named model resources. Release and reset
retain their distinct remove/destruction behavior. The 0x104-byte name object
retains its original model, performs the timed height cycle, switches display
models and issues the original two-pass draw with primitive flags and alpha.
The indexed-record owner is the asset class named `cSurfaceManager` in retail;
its 64 twenty-byte records use the original quadratic normalization constants.
Its factory is exported for the separate named-factory registry reconstruction.

The shared 0xe4-byte model binding loads through the real model cache, configures
shadow references and clip planes, transforms bounds, chooses lights and submits
model drawing. Removal releases bounds, shadow references and the cache or
privately owned model according to flags. Transform-component callbacks use the
inline component at owner+0x58; the two destruction thunks call their specific
retail destructors. Constructor quaternion normalization, bit-preserved wheel
parameters and the full record-copy callback ABI were checked against raw code.

The selector leases the real preview camera and lights, configures its viewport,
wraps/skips disabled choices and owns its display resources. Fourteen startup
initializers populate tuning storage. Frontend creation fills widget slots
3 through 17 using font two, including the car selector at slot eleven.
Destruction visits those slots in reverse. Menu resource load/release connects
the common/frontend widgets, pause textures, panel-set state and transition cover.

## Measurement and remaining uncertainty

Eighty-five new function extents were checked against raw control flow and the
available Ghidra bodies. Seventeen new helpers match exactly. Two scalar-deleting
destructors, 0x4391c0 and 0x43b8a0, are reviewed near matches at 95.65%: their
only extra instruction is `xor edx, edx` before a helper whose EDX argument is
unused. Calls, conditional frees, return values and stack cleanup match.
These nineteen functions use FUNCTION markers. Larger reconstructions remain
STUB, including the name animation/drawing despite an independent semantic
review finding no concrete branch discrepancy. Their low instruction similarity
and remaining floating-point/template details still warrant review. Some
allocation-failure paths in the template factories return early where retail
would subsequently dereference null; they are not claimed equivalent.

Seven explicit function-pointer tables are deliberately documented by plain VA
comments rather than data annotations. Generic data comparison misassociates
folded retail callback aliases; the complete scene CSV and verifier instead
check every original target and every emitted pointer through exact linker-map
symbols. This includes both surface-record tables, both object tables and all
three transform tables. The model-binding pair and car widget table are also
covered. This validates wiring, independently of callback behavior or scores.

Named-object factory registration, full controller startup and the foyer asset
path are still being integrated. The linked preview constructors alone do not
make startup reach a menu.

## Validation

Both pinned SP5 builders compile/link 363 units. All 1,701 measured scores and
statuses agree: 504 exact functions, twenty-one reviewed near matches and 1,176
STUB rows. All twelve dispatch checkers pass in both PEs; the scene/model check
covers 314 pointers and menu check covers 84 pointers. Twelve complete folded
aliases pass. Data comparison reports zero issues across 272 globals. The depth
threshold at 0x535470 is also verified as bytes `0e74da3c` in both builds through
their linker maps. All 21 retained tooling tests pass. No game behavior tests,
asset probes or runtime runs were performed.

Logs: `build/msvc/car-preview-build.log`, `build/msvc/car-preview-final-compare.log`,
`build/msvc/reports/car-preview-datacmp.txt` and corresponding CMake paths;
`build/msvc/car-preview-dispatch.log`, `build/msvc/car-preview-depth-storage.log` and
`build/msvc/car-preview-tooltests.log`. Unlinked worker annotations can produce
missing-symbol notices; they are outside the canonical measured count.
