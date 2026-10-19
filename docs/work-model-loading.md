# Model geometry, collision and shadow loading

The canonical build now includes the complete outer MDL reader at `0x004a7d40`,
its geometry parser at `0x004a8140`, COLD data relocation, SHA0 shadow loading,
and model/cache cleanup. These are dependencies of the foyer and car previews.
The temporary bootstrap remains active; this batch does not establish a
rendered menu or validated asset rendering.

## Geometry and ownership

The outer reader checks the model cache, creates the `0x48`-byte resource,
opens its file, enters deferred texture-loading mode, reads geometry and
optional SHA0/SKN0/COLD chunks, closes the file, flushes deferred loads and
finalizes primitive draw plans. Unknown chunks retain the original seek path.
The original open-failure allocation leak and zero-length COLD return value
of two are retained. Collision regeneration follows the application callback.

Raw instruction review corrected several details in the geometry parser:

- The VC6 primitive-array cookie advances four bytes before the first
  `0x3c`-byte primitive, rather than one entire primitive.
- GEO0 group types are initialized before the optional GEO1 overrides.
- The nine bounds floats contain minimum, maximum and translation vectors;
  the following yaw is a float. Corners use the recovered matrix operations
  before translation, center and radius calculation.
- Texture-stage selection preserves slot kinds, coordinate assignments and
  the original four-coordinate limit. Heap-data ownership is cleared at
  `+0x1c` in the vertex-data block.
- Index-buffer creation and locking retain the evidenced `(6, 0)` factory
  arguments and `(0, indexCount, 1)` lock arguments in their respective paths.
- The special-primitive test reads bit 3 of the defaults word at primitive
  `+0x14`, not the primitive flags at `+0x06`.

Cleanup releases material/texture references, collision storage, shadow
references, skin storage, reverse-ordered groups, renderer state, reference
storage and bounds. Cache removal shifts complete `0x88`-byte entries and
preserves the original unguarded lookup contract.

## Serialized collision data

The COLD header is `0x50` bytes with signature `0x82`, geometry at `+0x04`,
children at `+0x38` and runtime state at `+0x4c`. Relocation walks the single
allocation, assigns optional compound/simple geometry, sphere/box arrays,
triangle records, interleaved `0x18`-byte vertices and recursive children.
Triangle vertex references are remapped from serialized pointers. Compound
data recomputes bounds and the original axis/corner classification.

Five helpers match exactly: the service relocation wrapper, resource
relocation, geometry-storage relocation, compound relocation and simple
relocation. Other collision helpers remain `STUB`, with their measured scores
visible. The large bounds implementation has not been runtime-validated.

## Shared shadows

The SHA0 path reads face and edge records into the evidenced `0x38`-byte shadow
owner. Renderer storage at `+0x4b4` owns a shared primitive, material, texture
and reference count. Its texture uses the original shadow asset path; vertex
and index buffers use the real renderer factories. Model release drops the
appropriate geometry/texture references, then the shared renderer reference.
The original decrement and mask behavior is retained, including its lack of
an underflow guard. These large routines remain under review.

## Validation

Both VC6 SP5 builders compile and link 344 canonical units. The reports agree
on all 1,564 measured scores and statuses: 472 exact functions, nineteen
reviewed near matches and 1,073 `STUB` rows. All twelve existing dispatch
verifiers pass in both PEs. Eleven complete folded aliases remain verified.
Annotated-data comparison reports zero issues across 263 globals. Worker
sources outside the canonical manifest still generate unlinked-symbol notices.

All 24 new function extents were checked against control flow and the Ghidra
inventory. No game behavior tests were added, and no game, menu or asset probe
was executed. Foyer actions/assets, car-preview object bindings and controller
startup remain required before the next rendered-menu runtime milestone.
