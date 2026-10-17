# Renderer pipeline helpers

This unit recovers compact stencil and color-override helpers on
`DisplayRenderer`. Shared member and dispatch declarations live in
`renderer.h` and `rendererdispatch.h`; their offsets were confirmed from the
retail bodies and renderer constructor.

`SetStencilBufferEnabled` (`0x004dee20`) gates the stencil render state on the
preferred depth-format flag at `+0x24570`, then updates cached render state 52
(`D3DRS_STENCILENABLE`). `ResetStencilOperations` (`0x004dee90`) calls dispatch
slot `+0x94` for targets 0, 1 and 2 with operation 0. `SetStencilFunction`
(`0x004deed0`) maps selectors 0 through 7 to `D3DCMP_NEVER` through
`D3DCMP_ALWAYS`. `SetStencilOperation` (`0x004deff0`) maps targets 0 through 2
to `D3DRS_STENCILFAIL`, `D3DRS_STENCILZFAIL` and `D3DRS_STENCILPASS`; operation
selectors map from `D3DSTENCILOP_KEEP` through `D3DSTENCILOP_DECR`. Selector 2
also updates cached `D3DRS_STENCILREF` from its third argument.

`SetColorOverride` (`0x004df190`) stores the four low-to-high bytes of its
packed argument as normalized floats at `+0xc6278` through `+0xc6284`, and sets
the enable flag at `+0xc6274`. The renderer's color-application path at
`0x004dcf30` copies these four values when that flag is set. The paired
`DisableColorOverride` (`0x004df220`) clears the flag. `SetBlendModeOverride`
and `ClearBlendModeOverride` (`0x004df230` and `0x004df250`) set and clear the
existing blend override fields at `+0xc6288` and `+0xc628c`.

The isolated VC6 worker builds successfully. Raw scores are 40% for
`SetStencilBufferEnabled`, 81.63% for `ResetStencilOperations`, 13.51% for
`SetStencilFunction`, 17.33% for `SetStencilOperation`, and 15.38% for
`SetColorOverride`; these remain `STUB` because their code shapes differ
substantially despite the recovered behavior. The three direct flag/value
helpers score 100%: `DisableColorOverride` (11 bytes), `SetBlendModeOverride`
(23 bytes), and `ClearBlendModeOverride` (11 bytes). Their markers are
`FUNCTION`. Other extents are 98 bytes, 62 bytes, 248 bytes and 376 bytes for
the stencil methods in address order; jump tables after the two switch bodies
are data and excluded. `SetColorOverride` is 133 bytes. No game execution or
runtime probe is part of this batch.
