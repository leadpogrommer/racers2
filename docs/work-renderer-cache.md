# Renderer state caches and texture-stage reset

`renderercache.cpp` recovers the render-state setter at `0x004adfe0`
(46 bytes), the eight-stage reset at `0x004ae010` (759 bytes), and the
texture-stage setter at `0x004ae310` (66 bytes). These use the actual
`IDirect3DDevice8` methods and the renderer's observed state arrays.

The render-state cache begins at `+0xc734c` and holds `0xac` DWORDs. The
texture-handle cache has eight DWORDs at `+0xc75fc`. Texture-stage states
occupy 256 DWORDs at `+0xc7624`; their index is precisely
`(stage << 5) | state`, preserving the retail bitwise operation. Both setters
compare the cache, update it before issuing the API call, and ignore HRESULT.
They therefore suppress a repeated identical request even if the prior API
call failed. No null-device guard or bounds checks are added to the setters.

The reset returns immediately for a null device. Otherwise it processes stages
0 through 7 in order: unbind the texture, write `0x12345678` to that stage's
handle cache, disable color and alpha operations, select texture/current
arguments, clear the coordinate index, clamp U/V/W addressing, enable linear
minification/magnification/mip filtering, clear mip bias/maximum level/border
color, and disable texture transforms. Cache comparisons still control each
state call. Finally, stage zero's color and alpha operations become MODULATE.
The sentinel after an unbind is intentional; it is not normalized to zero.

The common renderer layout now gives the presentation block at `+0x7b8` its
real `D3DPRESENT_PARAMETERS` type, with its `0x34`-byte size asserted. Depth
format at `+0x7dc` is its `AutoDepthStencilFormat`; multisample type at
`+0x7c8` is its `MultiSampleType`. Render/cache fields keep independent offset
assertions. This remains an observed prefix, not a constructed renderer or
an emitted replacement game vtable.

The dispatch view names only observed slots while preserving every intervening
offset. It emits no table and supplies no substitute callbacks. The +0x198
policy used by vertex-buffer allocation retains the original ECX receiver
and stack layout through the same explicit x86 ABI convention as the
application dispatch bridges.

Combined reccmp measures `SetCachedRenderState` at 100%,
`ResetTextureStages` at 30.95%, and `SetCachedTextureStageState` at 43.48%.
The two low-scoring routines remain `STUB`; their readable state transitions
are retained without treating the scores as proof of equivalence. No game or
probe was run for this implementation batch.

An independent disassembly review confirmed the setters' failure ordering and
all reset-state values and call order. In both combined VC6 builds,
`SetCachedRenderState` matches 100% and uses `FUNCTION`. `ResetTextureStages`
measures 30.95%, and `SetCachedTextureStageState` measures 43.48%; both remain
`STUB`. The readable calls and loops were retained rather than polishing the
retail inlining and register choices. No game executable was run.
