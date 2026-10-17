# Renderer state and submission callbacks

The next renderer batch recovers quality selection (`0x4dc510`), fog and alpha
reference state, the depth-always-write callback, a nested solid-pass counter,
render-target switching (`0x4dfd80`), double-pass stencil-volume drawing
(`0x4dffb0`), per-stage texture transforms (`0x4e18a0`), and draw-plan
preparation (`0x4e9db0`). These remain STUB pending combined review.

The fog color callback reverses the low RGB bytes into D3D's color ordering
and ignores the high byte. Fog range arguments retain their float bits; alpha
reference clamps an unsigned argument to 16. The solid-pass counter uses
unsigned wraparound and the query also requires solid fill mode.

Render-target switching takes an actual texture-record pointer. It gets the
current depth-stencil surface before either branch, saves the original render
target and a pointer to the selected viewport, and uses level zero of the
record's texture. Restore clears the two saved pointers. Surface COM references
are not released in this retail body; the reconstruction preserves that behavior.

Draw-plan preparation invokes slot +0x84 for bump-map plans, reads the vertex
buffer's actual attribute callback at +0x28, sets bit 1 of the flags byte at
request+7 if attributes include bit 4, and invokes the plan resolver at +0x80.
The two unresolved false-return policies at `0x4abdd0` and `0x4ac0e0` preserve
three and six stack arguments respectively, as shown by RET 0x0c and RET 0x18.
Their working names retain uncertainty about the unused parameters.

# Sprite batching

`renderersprite.cpp` recovers the four-argument batch setup at `0x4df6b0`,
record dispatcher at `0x4df6e0`, four-vertex reservation at `0x4df720`, and
billboard projection/emission at `0x4df7d0`. The sprite prefix is 0x4c bytes,
with position at +0x10, packed color at +0x1c, texture ID at +0x20, UV rectangle
at +0x24, size fields at +0x34, rotation index at +0x40, depth bias at +0x44,
and blend mode at +0x48. The emitted vertex stride is 0x20 bytes.

Projection reads the actual singleton's cached world/view/projection/viewport
matrix. Retail's signed integer comparisons on stored float bits are retained,
including its near/depth and viewport bounds behavior. The source preserves
untouched specular color fields, UV ordering, reverse vertex finalization, and
lazy initialization of the 256-entry sine table. The sine generator reuses the existing exact `BuildSineTable` recovery. It uses the real shared storage at 0x578cc0 and guard at 0x5790c0.

Large sprite arithmetic is still under precision and accumulation-order review;
no semantic-equivalence or runtime-rendering claim is made from its presence.
No game or asset probe was executed.

Combined verification: the two false-return callbacks and solid-pass counter
match exactly. Texture-quality selection is a reviewed 97.14% near match: the
only difference is signed JL versus unsigned JB for the loop counter, which
is initialized to zero and only ranges from 0 through 8. The loop has no
negative counter state; every callback, store and argument is otherwise equal.
The broader state and submission paths remain STUB.
