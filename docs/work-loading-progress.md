# Loading screen and fade controller

The loading screen used during controller scene preparation now has linked
source, including textures, sprites, progress updates, equalizer animation and
the shared fade-controller lifecycle. This closes direct dependencies of the
staged controller startup. The temporary bootstrap remains active; no loading
screen or menu rendering has been observed.

## Layout and asset ownership

The existing `0x98`-byte selection-state constructor now uses the shared
`ControllerLoadingProgress` layout: active flag, selected world, progress,
elapsed/last-update times, five targets/values/velocities and eighteen sprites.
The untouched fields remain untouched by the constructor.

The loading texture owner uses flag `0x200` and event records 270 through 279.
Four background quadrants are followed by three progress-bar regions, the
localized loading word and two equalizer regions. World and language strings,
asset path formats and every UV word come from the original executable.
The background loader changes the final digit of `01.tga` to load four tiles.
Release drops all ten references in reverse order. Its unused mode argument
is retained: the original returns with `RET 4`, despite the decompiler omitting
the parameter. The corrected release helper matches exactly.

Sprite creation retains the four half-screen background quads, three progress
pieces, centered word and five two-piece equalizer bars. Their layer numbers
are six, seven and eight. Tuning storage at `0x0056be58` is initialized through
the recovered startup initializers; zero BSS contents are not treated as the
runtime dimensions. Untiled sprite tile-size fields remain uninitialized.

Updates use the signed clock, skip elapsed intervals at or below `1/30` second
unless forced, advance equalizer motion and perform the original begin/draw/
end/present/clear sequence. The equalizer uses the second float and integer
random streams and retains target clamping and velocity-based interpolation.
Large drawing/timing methods remain `STUB`; floating-point and instruction
differences are not claimed equivalent solely from successful compilation.

## Fade controller and preview bounds

The `0x5c`-byte SecondaryFX manager owns a `0x18`-byte state. Its twenty-slot
table includes the specialized initializer, remover, update and empty draw
callback. The initializer allocates the state and applies the real shared
zero-word initializer. Removal releases the overlay and storage before base
cleanup. The shared empty draw target also services a foyer parser directive;
it has one annotated definition and complete folded-body verification.

The state lazily creates its full-screen black overlay, supports immediate
clear/black modes and timed fades, and preserves the original completion and
release behavior. Loading-screen initialization clears this state before
creating its own sprites. Existing controller calls retain their raw duration
bits, while the state exposes a typed float API.

Review of the preview binding path confirmed that the existing GameObject
transform callback already calls the recovered position/rotation setters.
It also found a separate bounds-classification error: transformed X/Z bounds
must clear the elongated flag when the smaller/larger ratio is greater than
`0.5`. The original instructions and existing model-bounds classifier agree;
both inverted comparisons in child-geometry refresh are corrected.

## Validation

Both SP5 build systems compile and link 348 canonical units. The reports agree
on all 1,591 measured scores and statuses: 481 exact functions, nineteen reviewed
near matches and 1,091 STUB rows. All twelve dispatch verifiers pass in both PEs;
the scene check now covers 146 pointers. Twelve complete folded aliases pass,
and data comparison reports zero issues across 264 globals. The 21 retained
identification/match-tooling tests pass. These checks do not execute the game. Full controller/menu
construction, foyer actions/assets and car-preview bindings remain open.
