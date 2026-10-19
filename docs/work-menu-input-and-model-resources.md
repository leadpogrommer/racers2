# Input editor, text entry and model-resource dependencies

This batch connects the input-binding editor, its label selector initializer,
and the on-screen text-entry keyboard to the canonical build. It also recovers
model cache lookup/insertion, material and texture reference ownership, geometry
record construction/cleanup, model bounds, skin-file reading and deferred texture
loads. These are dependencies of the real menu and its character/car previews.
The temporary bootstrap remains active; this batch does not launch a menu.

## Frontend controls

The input editor occupies `0x18c` bytes. Its six callbacks at retail table
`0x0052fae4` are bounds, font/child-selector creation, child-selector destruction,
reset, input and clipping. The shared ordinary font setter and no-op cleanup
would leave its child selectors unconstructed. The recovered table uses the
actual specialized callbacks instead. The menu dispatch verifier now checks
all 78 pointers in thirteen complete widget tables in both rebuilt PEs.

Label storage is independently mapped: sixteen `0x24`-byte mode-3 records at
`0x0055e658`, two fallback records at `0x0055e8a0`, twenty-eight `0x28`-byte
translation records at `0x0055e8e8`, and 256 mode-1 records at `0x0055ed50`.
Static integer-store decoding of the original initializers supplied all rows.
`0x00415740` supplies row spacing `0.75f * 0.05f`; leaving it zero stacks rows.
The `-1` mode-table index reads the preceding zero BSS string index, so it
resolves localized string zero before following the ordinary fallback rules.

The duplicate-binding predicate at `0x00416aa0` compares its first argument
against pair word 1 (word 5 in the second scan) and its second against word 0.
The unusual second-scan displacement is preserved. Label-selector initializer
`0x0041bc20` has ten explicit stack arguments, `RET 0x28`, and positions its
arrows half a line height below the label origin.

The original dynamic-label scratch starts at `0x006ff400`, but its writable
extent is not established by the unbounded copy. The reconstruction uses
unannotated growable storage sized for the localized string, numeric suffix
and terminator. Its allocation/address behavior and added missing-object guards
are documented differences; these functions remain `STUB`.

Text entry occupies `0x1fc` bytes: ten UTF-16 words at `+0x48`, text/cursor
record arrays at `+0x5c`/`+0x7c`, length at `+0x9c`, keyboard characters at
`+0xa0`, keyboard record pointers at `+0x104`, and three sprites at `+0x1cc`.
The append helper passes a terminated two-word local string. Initialization
clears all four alphabet rows even when fewer rows are used, then creates the
digit row. Text rows sit below the action sprites. Action-to-keyboard columns
are 4, 8 and 9; navigation wraps each candidate before testing an empty cell.
The three action sprites copy controller event records 175 through 177.

Text-entry reset, character deletion, accepted-text query and clipping match
exactly, as do three of its tuning initializers. The larger layout/input
methods remain under review. The original nonempty localized-key-string
assumption is preserved; this has not been checked by running the UI.

## Model resources

The loader at `0x004a7d40` establishes the model's complete `0x48`-byte layout.
Groups at `+0x0c` have `0x14`-byte stride; their primitives have `0x3c`-byte
stride. A primitive owns heap data at `+0x08`, vertex/index-buffer pointers at
`+0x0c`/`+0x10`, and four eight-byte texture slots at `+0x1c`. Slot texture IDs
are at `+0x04`. These agree with the renderer draw-packet and stage-plan views.
The existing draw-packet release function now exposes that common view for
model-group cleanup; no second implementation is substituted for its address.
The original VC6 primitive-array cookie is retained during release.

The `0x11004`-byte model cache at `0x00579168` contains a count followed by 512
`0x88`-byte entries (model pointer, 128-byte path, reference count). Normal-mode
lookup ignores case. Development-mode lookup also accepts the observed final
`~` suffix. A null entry terminates the count-bounded search. Insertion retains
the original unguarded full-cache behavior, and add-reference assumes a cached
model. Assignment, insertion and add-reference match exactly.

The model's eight-byte reference table at `+0x2c` stores texture IDs followed
by material IDs as signed 16-bit values. Release drops materials in reverse
order, then textures in reverse order, without freeing the ID allocation;
free-storage is a separate operation. Release, reset and four accessors match
exactly. The group constructor and all five newly recovered vector arithmetic
helpers also match exactly.

Bounds occupy `0x74` bytes: eight corners, the X/Z aspect classification, center
and radius. Skin resources occupy `0x20` bytes and read blend data plus `0x0c`-byte
lookup/position groups. Their position arrays are allocated without being read
from the skin chunk. The original untouched fields are not initialized by the
reconstruction. Large bounds, skin and geometry methods remain `STUB`.

`DisplayRenderer::SetResourceLoadMode` retains batching: mode zero walks current
resource references and invokes load slot `+0x04` only for resources whose
flag bit 1 is set. The full model geometry parser, outer chunk reader, shadow
and collision fixup paths are still being connected. The new dependency types
and leaf helpers do not establish complete model loading.

## Validation

Both VC6 SP5 builders compile and link the 338 canonical translation units.
Instruction comparison covers 1,540 measured rows, with 467 exact and nineteen
reviewed near-match `FUNCTION` claims. Another 1,054 rows remain `STUB`.
All twelve existing dispatch verifiers pass in both builds, including the
expanded thirteen-widget check. Eleven folded aliases retain complete-byte
and linker-map verification. Annotated data comparison covers 263 globals with
zero issues. The 21 existing identification/match-tooling tests pass.

No new game behavior/source tests were added. No game, menu or asset probe was
executed. Car preview, Foyer parsing/events and complete controller/startup
integration remain required before the next rendered-menu runtime milestone.
