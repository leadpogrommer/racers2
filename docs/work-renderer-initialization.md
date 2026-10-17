# Renderer initialization dependency audit

This is a read-only audit of retail `DisplayRenderer` construction at
`0x004ab9e0` and initialization at `0x004ac130`, cross-checked against the
current source tree. No renderer constructor or initializer is currently
implemented. Static startup work has reached application/configuration and
archive initialization, but `bootstrap.cpp` remains token-predicate
scaffolding and there is not yet a path to a real window or D3D device.

## What the retail path does

The static renderer object begins at `0x0058b140`. Its 967-byte constructor
first constructs two embedded lookup-table states, installs their derived
tables at object offsets `+0x04` and `+0x2c`, installs a third at `+0x54`,
initializes several large arrays of renderer-specific records, then installs
main vtable `0x00535554`. Its final four-record loop starts at
`this+0xc7f24` for the fourth record and writes its last byte at
`this+0xc7f2b`; required storage is therefore at least `0xc7f2c` bytes. The
current `DisplayRenderer` assertion is `0xc7efc`, below that proven bound.
Its comment already records the `+0xc7f2b` evidence, but the projection and
assertion must be extended before calling the constructor on static storage.
The initializer separately accesses state through `+0xc7f13`.

The initializer is 3,013 bytes and returns failure along several real error
paths. It obtains application display settings and title, applies command-line
overrides, allocates the 0x3c-byte renderer configuration at global
`0x0065307c`, creates the native window, saves and changes the x87 control
word, creates Direct3D 8, enumerates/selects an adapter and display mode,
builds presentation parameters, tries `CreateDevice` configurations, and
then initializes renderer texture formats, cached render states, and drawing
resources. `CreateVertexBuffers` at `0x004acfb0` is already implemented and
reviewed as a 95.33% raw near match; the initializer reaches it only near the
end, after device and render-state setup.

The main vtable at `0x00535554` is real PE data, 0x1a0 bytes (slots `+0x00`
through `+0x19c`); its final entry is purecall. Relevant slots called directly
by the initializer map as follows:

| Slot | Retail target | Current source state |
| --- | --- | --- |
| `+0x1c` | `0x004ae5c0` | `DisplayRenderer::SetMaterialColor`, source present as STUB |
| `+0x20` | `0x004dc9b0` | `DisplayRenderer::ClearBuffers`, source present as STUB |
| `+0x24` | `0x004dfb20` | `DisplayRenderer::Present`, FUNCTION |
| `+0x2c` | `0x004ddf30` | No source implementation |
| `+0x3c` | `0x004bf150` | No source implementation |
| `+0x44` | `0x004e94b0` | No source implementation |
| `+0x80` | `0x004e24f0` | No source implementation |
| `+0xb0` | `0x004ae360` | No source implementation |
| `+0xb8` | `0x004ae430` | No source implementation |
| `+0xd4` | `0x004d9f20` | No source implementation |
| `+0x100` | `0x004e9e10` | No source implementation |
| `+0x114` | `0x004ac040` | No source implementation |
| `+0x198` | `0x004ac120` | `UsesHardwareVertexProcessing`, source present |

These are entries in the existing retail table, not proposed replacements.
The current dispatch header is a partial offset view; it cannot stand in for
the full table. The default-state path also needs the observed `+0x98`,
`+0xc8`, and `+0x124` targets (`0x004dc7f0`, `0x004dccc0`, and `0x004e0560`)
to be represented by real bodies and wired to the actual table.

## Missing direct callees and smallest useful work order

The constructor's non-library callees without current source bodies include
base table constructor `0x004abdb0` (26 bytes); record constructors
`0x004abe50` (44), `0x004abea0` (9), `0x004abde0` (74), and `0x004abe30`
(29); and record initializer `0x00479490` (52). Their callers show repeated
arrays: 800 records at `0x004abe50`, 5,000 each at `0x004abde0` and
`0x004abe30`, and 256 0x14-byte elements initialized through `0x004abea0`.
`0x004abe90` and `0x004abeb0` already have FUNCTION bodies. `0x00401020`
(42 bytes) is the compiler's array-constructor iterator helper, not a
game-authored renderer routine; identify and reuse its compiler/runtime
implementation instead of creating a renderer function there. The missing
element layouts and their vtables still need to be declared from observed
strides and writes;
the helpers should not be replaced with zero-filled arrays because several
constructors preserve bits or install specific vptrs.

After the owner layout and constructor are closed, the initializer's direct
non-library callees that still lack any source body are:

| VA | Bytes | Evidence / role in this path |
| --- | ---: | --- |
| `0x004e1960` | 712 | Initializes a separate large subsystem and calls `0x004e1dd0`; touches global object `0x0058b960`. |
| `0x004c0b70` | 840 | Builds a renderer resource from a record and several buffers; called while installing the initial scene/draw resource. |
| `0x004ec2c0` | 258 | Initializes a renderer-owned object; has additional calls to `0x004c11e0` and `0x004c0770`. |
| `0x004a7cc0` | 50 | Releases a heap field and invokes slot-zero destructors on two embedded pointers. Its owner type is not established. |

The other direct calls are either available or identifiable APIs: application
singleton/configuration (`0x0049a760`, currently STUB-annotated source), heap
allocation (`0x004a4190`), command-line value helpers (`0x004b4a60` and
`0x004b4b30`), window creation (`0x004b46c0`, source present as STUB), FPU
control (`0x004e7b30` exact and `0x004e7b10` STUB-annotated), Direct3D 8
creation and COM methods, adapter/mode selection (`0x004ad2c0`, `0x004ad540`,
`0x004ada60`), depth format selection (`0x004aebb0`), NVIDIA policy
(`0x004acd00`), texture format discovery (`0x004d8840`), default state setup
(`0x004adba0`), and vertex-buffer setup (`0x004acfb0`). Several have source
bodies but remain STUB-annotated, so their retail branch behavior and return
contracts still need review before calling them complete.

Texture discovery has another direct support chain: `0x004d8840` calls
`0x004d8800` and `0x004da240`, both currently STUB-annotated, and the former
calls the pixel-format descriptor initializer at `0x004ae660`, also
STUB-annotated. The default-state setup reaches `0x004dc7f0`, `0x004dccc0`,
and `0x004e0560` through renderer slots. These are practical follow-up leaves
once the constructor and D3D device path are in place.

## Recommended closure order

1. Extend the evidenced renderer storage projection through `+0xc7f2b`, then
   recover the small array/record constructors and their exact element
   strides. Implement the 967-byte owner constructor only after those vptrs
   and record layouts are supported.
2. Complete the owner vtable entries directly required to make a windowed
   device and default render state: adapter/mode helpers, selected-device
   configuration, `+0x2c/+0x3c/+0x44/+0x80/+0xb0/+0xb8`, texture/depth format
   selection, and state slots `+0x98/+0xc8/+0x124`. Keep the full table tied
   to retail data rather than emitting an abbreviated table.
3. Close `0x004e1960`, `0x004c0b70`, `0x004ec2c0`, and the owner of
   `0x004a7cc0`; then integrate `0x004ac130` and exercise a real menu-render
   milestone. Until then, successful compilation or a placeholder window
   does not establish renderer readiness.

The highest-risk unknown is not the Direct3D SDK surface, which is already
available through `util/directx8.h`; it is the game's own large nested record
layouts and the missing renderer dispatch methods that construct the scene
resources. The exact 0x535554 table and retail call sites provide a direct
roadmap for those closures.
