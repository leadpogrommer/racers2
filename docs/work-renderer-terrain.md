# Renderer terrain callbacks

`rendererterrain.h` records the 0x14-byte output packet observed by the six-
argument `stdcall` callback at 0x004eb9d0. The callback clears the two 16-bit
count fields at +0/+2, reads the vertex-buffer pointer at +0x0c and the
index-buffer pointer at +0x10, and appends terrain vertices and indices. The
first object exposes the `RendererVertexBuffer` operations GetCount,
GetStride, GetVertex, and SetCursor; the second exposes its index data at
+0x10.

`BuildTerrainGeometry` remains `STUB`. Its terrain transform, projected
bounds, grid clipping, height sampling, and vertex positions follow the
observed field accesses and coordinate order. The binary has two distinct
index-generation paths: a regular 7-level path and a separate
hole/alternating-topology path. The source currently emits triangle lists for
both, so it does not preserve the retail strip order, degenerate connector
indices, or exact hole transitions. Build success does not establish
equivalence.

The other terrain callback is `DisplayRenderer::DrawTerrain(Terrain*)` at
0x004ea690, length 3,741 bytes through its three-byte return at 0x004eb52a.
Its direct game-code dependencies include 0x004bf860 (directional-light
component reduction), 0x004c06d0 (application auxiliary-resource lookup),
0x004cb530, 0x004d42e0, 0x004ea550, and 0x004eb530. It also uses renderer and
device virtual slots throughout the terrain pass.

Source implementations now exist for the directional-light reduction
(0x004bf860, 157 bytes), terrain LOD thresholds (0x004cb530, 243 bytes), and
per-tile edge selection (0x004ea550, 308 bytes). They remain `STUB` pending
integrated raw measurement. The threshold helper uses renderer dispatch slot
+0x58, renderer detail level +0x4d0, constants read from the PE, and terrain
values +0x1f0 and +0x1dc..+0x1ec. The edge helper derives four flags from
neighboring signed LOD bytes and delegates height updates to the recovered
`TerrainVertexGrid::SelectEdges` method.

`TerrainVisibilityContext` is an opaque receiver view for the cell-state
query at 0x004d42e0. Only its `thiscall` member ABI with two stack arguments
and `ret 8` is asserted; no complete allocation size is claimed. The helper
walks a per-level 0x20-byte node table and packed two-bit overlays, including
code-3 fallback to coarser levels and overlay aggregation. It also remains
`STUB` until integrated measurement and review.

`DisplayRenderer::BuildTerrainGridStream` is now present as a six-stack-argument
`thiscall` method with `ret 0x18`, extent 0x004eb530..0x004eb9c6 (1,175
bytes). It streams the 0x24-byte vertex format, uses the caller's cumulative
vertex-base slot for 16-bit indices, selects the retail grayscale table at
0x0054b0bc in hardware mode, and distinguishes regular topology 7 from the
hole-flag path. Its source remains `STUB`: the isolated raw score is 6.25%,
and branch/layout code generation differs substantially. The 0x38-byte
zero-initialized stream scratch block at 0x006e4518..0x006e454f and its
per-tile offset writes are now represented in the source. The grayscale table
was read from the original PE and contains sixteen ARGB entries from
0xff000000 through 0xfff0f0f0 in 0x101010 increments.

`DisplayRenderer::DrawTerrain` now has an initial source implementation. It
uses the observed transform, auxiliary-material and ambient-color helpers,
queries per-tile visibility and LOD, selects edge samples, locks the streaming
vertex/index buffers, calls `BuildTerrainGridStream`, and submits indexed strips.
The body remains `STUB` and is not a complete reconstruction: it submits each
tile independently, does not preserve retail's grouped multi-tile batches or
the hardware/software vertex-processing branches, and simplifies the original
render-state/cache transitions. Its isolated raw score is 13.14%.

The two material helpers are implemented at 0x004dd0b0 and 0x004dfec0. The
first copies the indexed auxiliary record into renderer storage at +0x24528,
sets the device material, updates cached specular state 0x1d, and dispatches
the record mode through renderer slot +0xc8. The second saves four floats,
packs their channels with the active x87 rounding mode, and updates
D3DRS_AMBIENT only when its cached value changes. They remain `STUB`; isolated
raw scores are 89.41% and 65.55%, respectively.

The terrain worker builds and links. Direct `asmcmp` supplied the scores above;
the repository `compare.py` wrapper stopped earlier at an unrelated
`function_out_of_order` error in `rendererlookupflush.cpp`, so no full lint
result is claimed for this checkpoint. No runtime validation was run.

The tile address formula at 0x004ca850 is `tiles[tileRows * column + row]`;
it differs from the higher-level lookup at 0x004cb680, which derives tile
coordinates from map-grid coordinates. The shared `Terrain` class declares
only a partial prefix, so renderer-only fields beyond +0x1d0 remain offset
projections rather than a guessed complete object layout.

Root integration corrected the transform receiver dereference at terrain+0x54, the fixed ambient alpha of 1.0, initial material-source render-state IDs 0x93/0x91/0x8d, and the normal-buffer pointer at renderer+0xc7ee0. Both main builders now compile and lint this unit. Integrated raw scores are 91.57% for ApplyAuxiliaryMaterial, 68.42% for SetAmbientColorFloats and 10.21% for DrawTerrain; all remain STUB. The lower draw score accompanies correctness fixes and is not a reason to retain the earlier wrong offsets. The terrain scratch and grayscale palette pass data comparison. The batching, topology, software-processing and state-restoration limitations above remain unresolved.
