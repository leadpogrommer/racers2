# Texture-stage execution and transforms

The stage-plan execution path is recovered in `rendererstageapply.cpp` and
`renderertexturetransform.cpp`. This closes the operations used by the
renderer validation path at `0x004e5f90`; it has not been game-tested.

| Retail address | Bytes | Working name |
| --- | ---: | --- |
| `0x004ddeb0` | 123 | MultiplyTextureMatrixInPlace |
| `0x004dfb60` | 418 | ApplyTextureTransform |
| `0x004dfd10` | 99 | UpdateTextureStageFlags |
| `0x004dff90` | 7 | GetStageFactorColor |
| `0x004dffa0` | 14 | GetInverseStageFactorColor |
| `0x004e1950` | 5 | PassThroughStageValidationResult |
| `0x004e1e20` | 59 | ApplyStagePlanSubrecord |
| `0x004e2490` | 37 | ResetTextureStageCoordinateIndices |

The execution algorithm from `0x004e2160` is currently an unannotated helper
with an explicit renderer argument. Retail accesses the static renderer;
the reconstructed caller recovers that same owner from its embedded stage
manager at `+0x25148`. This avoids independent copies of the retail renderer's
state while its complete constructor and dispatch table remain unresolved.
An original-ABI wrapper and measurement of `0x004e2160` await that owner.

The linked stage-record list supports direct render-state and texture-stage
writes, factor/inverse-factor colors, and three alpha/blend operations. Alpha
conversion preserves the stored single-precision intermediate followed by
FISTP using the current x87 rounding mode. The empty call at `0x00479030`
in the two factor-color branches has no state effect. Active texture stages
are applied in descending order. Signed descriptor indices, upper-nibble
coordinate flags and the original null-descriptor failure behavior remain.

The renderer projection now names the address-mode table at `+0x4fc`, alpha
at `+0x24544`, factor color at `+0x2459c`, the transform enable flag and ten
0x44-byte transforms at `+0xc70a0/+0xc70a4`, and blend at `+0xc7a2c`. Existing
cache arrays provide the actual device state. The three matrix/state globals
at `0x0054ae10`, `0x0054ae30`, and `0x0054ae70` were read from the PE, with
values preserved exactly. Cube and ordinary textures take the different
matrix branches visible in retail, including the distinct 0.25 and 0.5 scale
constants. Matrix multiplication uses a temporary for aliasing and preserves
the observed accumulation order (products 0, 1, 3, 2).

No complete renderer singleton is constructed by these units, and no runtime
success is synthesized. Function claims await the integrated build/msvc/report.

The first integrated comparison measured `UpdateTextureStageFlags`, both
factor-color getters and `PassThroughStageValidationResult` at 100%; these
are now FUNCTION claims. Other raw scores were 48.35% for matrix multiply,
24.90% for texture-transform application, 34.62% for the manager subrecord
application, and 92.86% for coordinate reset; these remain STUB. The three
new constant tables pass static data comparison (71 globals, zero issues).
`SupportsCubeTextures` also matched 100%; shared-memory multitexturing
capability checks measured 82.76% and remain STUB.

The renderer shutdown path at `0x004ad060` (198 bytes) now has source in
`renderershutdown.cpp`. It shuts down streaming, invokes real slot-zero
callbacks on live vertex/index records, releases all 16 viewport slots,
releases Direct3D/device interfaces, restores FPU state and cursor visibility,
and frees the shared display configuration. Viewport release at `0x004ae5a0`
is 27 bytes. Neither is called before full owner integration. The callback
views describe the observed ABI without constructing replacement tables.
