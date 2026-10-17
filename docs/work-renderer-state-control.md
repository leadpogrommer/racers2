# Renderer state-control dispatch

This batch recovers two renderer dispatch operations with direct caller and
cache evidence. The renderer dispatch view identifies slots `0x98`, `0xc8` and
`0x124`; `InitializeDefaultRenderStates` calls the first two with mode `1` and
`0`, respectively, and later calls slot `0x124` with `0`.

`0x004dc7f0` is a one-argument `thiscall` operation (198 bytes, ending in
`ret 4`). `SetDepthState` enables depth testing only when its argument equals
exactly `1`. That path sets cached D3D render states `ZENABLE=1`,
`ZWRITEENABLE=1` and `ZFUNC=4`; every other argument sets only `ZENABLE=0`
and `ZWRITEENABLE=0`, leaving the depth comparison function untouched. The
existing `SetCachedRenderState` method preserves the observed update-before-D3D
call and avoids a device call when the cached value already matches.

`0x004dccc0` is also a one-argument `thiscall` operation (583 bytes, ending in
`ret 4`). If the observed override flag at `+0xc6288` equals `1`, the method
uses the value at `+0xc628c`; otherwise it uses its argument. Mode zero disables
alpha blending, enables alpha testing and enables depth writes. Modes 1–7 select
source/destination blend factors `(5,6)`, `(2,4)`, `(5,2)`, `(2,2)`, `(1,6)`,
`(1,4)` and `(5,6)`, then enable alpha blending, disable alpha testing and
disable depth writes. Other nonzero modes skip changing the blend factors but
still take that common nonzero path. The factor values are the D3D8
`D3DBLEND_*` numeric values, confirmed by the device calls using render states
19 and 20. State caches are at `+0xc7398` (source blend), `+0xc739c`
(destination blend), `+0xc73b8` (alpha blend), `+0xc7388` (alpha test) and
`+0xc7384` (depth write).

`0x004e0560`, corresponding to slot `0x124`, has a one-argument `thiscall`
signature and ends at `0x004e0ce2` with `ret 4` (0x783 bytes). It is not
implemented here. Its selector dispatch has a special input value 14 that
consults global `0x0067bd70` and maps that value before a second switch. Across
the cases it mutates multiple render-state and texture-stage caches, calls
device methods at several vtable offsets, and invokes renderer helper
`0x004e1e60`; that helper in turn calls `0x004e1fa0` and touches texture and
stage-state caches. The available evidence does not yet establish the meaning
of all selector values or the complete field layout used by those helpers, so
slot `0x124` remains an explicit dependency for future default-state/runtime
work rather than a guessed callback.

The two source bodies remain real, behaviorally reviewed implementations, but
are marked `STUB` because delegation through `SetCachedRenderState` yields only
26.67% raw similarity for `0x004dc7f0` and 8.94% for `0x004dccc0`; neither meets
the repository's threshold for a `FUNCTION` claim. This bounded pass did not
attempt to inline the state-cache logic solely to chase those scores. No runtime
or game behavior tests were run.
