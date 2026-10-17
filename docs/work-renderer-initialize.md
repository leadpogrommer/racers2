# Renderer initialization work

`DisplayRenderer::Initialize` (retail `0x004ac130`, 3013 bytes including its
return) now follows the recovered startup path through adapter selection,
Direct3D 8 device creation, initial render state, default texture setup,
quad/index buffers, stream setup, and final draw-packet initialization. The
body remains `STUB`: several table callbacks and dependent renderer routines
are still incomplete, and its isolated raw instruction match is 22.87%. It must not
be treated as a recovered or runnable renderer yet.

The initializer’s calls were checked against the raw disassembly and the local
DirectX 8 COM slot reference. In particular, viewport creation and binding use
renderer slots `+0xb0` and `+0xb8`; the post-texture-manager stencil toggles are
direct `IDirect3DDevice8::SetRenderState(0x1f, 1/0)` calls. Fullscreen
multisample selection stores the chosen type only after a successful
`CheckDeviceMultiSampleType`. Windowed and fullscreen fallback paths preserve
the original failure returns and requested-mode mutations.

The local draw packet used by the final `+0x80` renderer callback has four
leading words, three pointer fields, and the observed 0x28-byte defaults record
starting at offset `0x14`, for a total of `0x3c` bytes. Its cleanup helper at
`0x004a7cc0` frees the optional heap block, then calls the first callback of
the `+0x10` and `+0x0c` objects in that order. Those object fields remain
opaque because initialization zeroes them before dispatch.

The default texture record is created with an empty path and configured from a
4 by 4, 32-bit white `BitmapImage`. The initializer’s texture-record callback
at record slot `+0x24` receives that image. Renderer slot `+0xd4` receives an
output `BOOL*`; its exact descriptive operation name remains uncertain. The
white pixels live on the initializer’s stack and are consumed synchronously by
the callback.

An isolated VC6 SP5 build with the project’s Wine DirectX 8 configuration
passes. No game or probe was run. The remaining semantic and startup blockers
include renderer table callbacks and texture-stage/resource initialization;
the current source deliberately keeps the original failure paths and does not
claim successful menu rendering.
