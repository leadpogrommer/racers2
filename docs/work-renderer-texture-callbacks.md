# Renderer texture-record callback work

This unit reconstructs the ten-entry dispatch table at `0x005358e4` and the
texture-record loader/release paths that it reaches. The table is represented
as an ordinary typed data structure, not a compiler-emitted C++ vftable. Its
entries point to explicit `__fastcall` bridges whose ECX receiver and ignored
EDX preserve the retail thiscall layout.

| Retail address | Extent | Implementation |
| --- | ---: | --- |
| `0x004abe50` | 44 bytes | `RendererTextureRecord` constructor |
| `0x004bf920` | 308 bytes | `LoadMipSidecar` |
| `0x004bfa60` | 228 bytes | `EnsureMipSidecar` |
| `0x004bfd70` | 568 bytes | `RendererTextureRecord::Load` |
| `0x004c0050` | 22 bytes | `ReleaseRendererTextureResource` |
| `0x004da1e0` | 88 bytes | `DisplayRenderer::ReleaseTextureRecord` |

The helper call contracts are visible in the retail instructions: `0x4bf920`
receives the record in ECX and `(path, image)` on the stack. `0x4bfa60` also
receives the record in ECX and the same two stack arguments, although it does
not read the receiver. Renderer slot `+0xd8` takes `(path, suffix, image)`,
while slot `+0xdc` takes `(path, image)`. The latter second argument is easy to
miss in the decompiler output; the original pushes both values before the
indirect call.

`Load` retains the retail distinctions between resource type 3, ordinary
resources, and type 9 cube textures. It clears only flag bit 1 before checking
the path, uses the high-resolution policy for the six cube-face suffixes, and
releases temporary image storage along the same success/failure paths. The
ordinary upload callback receives the resource type sign-extended from its
16-bit field, matching the retail `movsx`. The
release callback uses the record's signed 16-bit resource ID and the renderer
selected by the actual global renderer pointer. The record constructor does
not initialize that ID; reserved static texture records therefore must not be
assigned an owner by deriving one from their ID.

The renderer owner and texture-record methods required by this unit are in the
shared headers. `renderertexturecallbacks.cpp` has linked successfully in its
isolated MSVC/Wine build. The new bodies remain `STUB` pending root integration,
raw reccmp measurement, and review; no match percentage is claimed here.
