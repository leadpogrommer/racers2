# Renderer vertex buffers

The bounded state slice recovers `DisplayRenderer::UsesHardwareVertexProcessing`
at `0x004ac120` and `DisplayRenderer::CreateVertexBuffers` at `0x004acfb0`.
The getter returns whether bit `0x20` is clear in the device behavior flags.
The renderer initializer selects `0x20` for software vertex processing and
`0x40` for hardware vertex processing, so the inferred getter name is supported
by the flag values passed to `IDirect3D8::CreateDevice`.

The buffer method creates a specular buffer of `0x10000` bytes with FVF `0x1c4`,
then a normal buffer of `m_vertexBufferCapacity * 36` bytes with FVF `0x152`.
Both use `m_vertexBufferUsage | 0x200` and `D3DPOOL_DEFAULT`. It then calls the
observed renderer dispatch slot at vtable offset `0x198`; when that returns
false, it creates the transformed buffer using `m_vertexBufferCapacity * 28`
bytes and FVF `0x144`. The HRESULTs are ignored and the method returns `TRUE`
unconditionally, matching the retail control flow. The slot call is retained
through the observed dispatch view; the only remaining raw instruction
difference is how the compiler chooses the caller-clobbered `EDX` scratch
register before the call.

The shared `DisplayRenderer` prefix now models the three output pointers and
the two input fields through `+0xc7efb`, ending at `+0xc7efc`. The usage word
at `+0x2456c` is `D3DUSAGE_WRITEONLY` or zero; buffer creation adds `0x200`.
Retail method extents are 15 bytes
(`0x004ac120` through the return at `0x004ac12e`) and 169 bytes
(`0x004acfb0` through the return at `0x004ad058`). Isolated reccmp measured the
getter at 100% and the buffer method at 95.33%; the latter's remaining difference
is limited to the caller-clobbered `EDX` value used to hold the vtable pointer.
No game runtime test was run; validation is limited to isolated compilation
and reccmp.

## Default device render state

`DisplayRenderer::InitializeDefaultRenderStates` (`0x004adba0`, 1084 bytes,
through `0x004adfdb`) has a source reconstruction of the observed state setup.
It fills 172 cached render-state dwords, 256 texture-stage cache dwords, and
eight texture-handle dwords with `0x12345678`, then clears the two observed
counters. If a device exists, it seeds the observed render-state cache and
issues changed states via `SetCachedRenderState`. The requested bit depth
selects the dithering value;
the presentation multisample field gates `D3DRS_MULTISAMPLEANTIALIAS`. The
initializer then calls the renderer dispatch methods at offsets `0x98`, `0xc8`
and `0x124`, with values 1, 0 and 0 respectively, resetting texture stages
between the latter two calls. The null-device path skips device-state writes but
still takes this common tail.

The initializer’s direct helper, `DisplayRenderer::SetTextureFiltering`
(`0x004dc590`, 333 bytes through `0x004dc6dc`), reads the selected adapter’s
`TextureFilterCaps`. Argument exactly 1 requests linear minification and
magnification where supported; all other values request point filtering where
supported. It applies each supported choice to all eight stages through the
existing stage-state cache. The combined builds measure the 1084-byte
initializer at 60.53% and the 333-byte texture-filter helper at 45.95%; both
remain `STUB`. The state order and branch effects were reviewed against retail,
but source-level helper calls and loop shape still differ substantially. No
game or probe was run for this implementation batch.
