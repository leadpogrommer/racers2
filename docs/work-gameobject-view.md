# Game object view preparation

`UpdateGameObjectRenderGeometry` models the no-explicit-argument camera/view
preparation routine at `0x004ba3b0`. Its caller first publishes the renderable
camera object at `0x00679ef0`; the routine reads the camera prefix from
`+0x188` through `+0x31c`, marks geometry usable at `+0x1c0`, derives the
horizontal and vertical field of view from the stored dimensions and scale,
selects the stored renderer viewport, then prepares the five culling-plane
entries beginning at `+0x270` and `+0x2b8`. The function ends at `0x004ba806`
and measures 1111 bytes including its `ret`.

For `height < width`, or when the alternate projection scale is zero, retail
uses the horizontal field of view as its base and derives the vertical angle
from `height / width`. Otherwise it uses the vertical angle as its base and
derives horizontal from `width / height`.

The raw body uses `atan`, `tan`, `sin` and `cos` in x87, with intermediate
single-precision stores in the culling setup. The current C++ reconstruction
uses the same branch and geometric formulas, but it does not reproduce all
x87 intermediate precision or confirm every transformed-plane alias. It stays
`STUB` until those details are reviewed. The two angle constants are the
float values read from `0x00535fe4` (pi) and `0x00530c58` (1/180); the shared
half-angle value at `0x0052f408` is 0.5.

The region visibility routine at `0x004d3a60` has an owner receiver and one
explicit camera/view pointer. It ends at `0x004d3e05` and is 934 bytes including
`ret 4`. It rebuilds the region grid at `0x004d46a0`, tests neighboring
candidate cells through `0x004d3f00`, and can submit terrain geometry through
the renderer callback. The receiver layout is still represented as an
owner-relative projection because its complete type has not been established.

The current owner-relative source reconstructs the grid/frustum rebuild at
`0x004d46a0` (566 bytes, `ret 4`). `ClassifyTerrainRegionCell` at
`0x004d4520` (357 bytes, `ret 4`) now models the five-plane rejection and
eight-point cell sampling, while `UpdateTerrainRegionCellNeighbors` at
`0x004d48e0` (367 bytes, `ret 12`) refreshes child cells recursively.
`BuildTerrainCellCorners` at `0x004d5cd0` (201 bytes, `ret 4`) fills the shared
sample array. These functions remain `STUB`; floating-point expression order
and the sample array's dynamic occlusion semantics still need review.

The candidate refresh is `0x004d3f00` (406 bytes, `ret 4`), not a 1172-byte
routine. It walks pending grid candidates, invokes the ray traversal at
`0x004f88f0`, and appends matching terrain entries. Its ray-cell callback is
`0x004d3e60` (156 bytes, plain `ret`). This traversal and callback remain
unimplemented; they depend on the terrain grid-cell accessor at `0x004ca850`
and ray-line stepping at `0x004f8ab0`.

The owner view currently models only fields read or written by this slice:
the level-record pointer array occupies `+0x68..+0xa7`; grid shift is `+0xa8`,
and the terrain geometry packet pointer is `+0x1a0`. These offsets are based on
direct owner-relative accesses and do not claim an owning class or allocation
extent.
