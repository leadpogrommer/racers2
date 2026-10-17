# Renderer resource policies

This batch reconstructs six small renderer policy callbacks. Their working
names describe the observed behavior; they do not claim recovered source
symbols.

`SetTextureIndexMap` at `0x004db900` stores the second argument at
`m_textureIndexMap[index]` and performs no bounds check. The array begins at
renderer offset `0x22cdc`. The body ends with `ret 8`, so it is a two-argument
`thiscall` method.

`ShouldUseHighResolutionTextures` at `0x004dc4c0` returns true only when the
signed physical-memory value at `+0x81c` is at least `0x09600000`, the signed
value at `+0x4d8` is below 4, the renderer's dispatch callback at `+0x184`
returns a value no greater than zero, and the signed available-texture-memory
value at `+0x814` is at least `0x01000000`. The callback is invoked through the
existing renderer dispatch view to preserve virtual dispatch.

Three one-argument callbacks classify resource kinds. At `0x004e1f20`, kind 2
is accepted; kind 3 is accepted only when DOTPRODUCT3 is unsupported; other
kinds are rejected. At `0x004e1f50`, kinds 1 and 9 are rejected, kind 3 is
accepted only when DOTPRODUCT3 is unsupported, and all other kinds are
accepted. At `0x004e1f80`, only kind 3 is special. The DOTPRODUCT3 query at
`0x004e6380` reads `D3DCAPS8::TextureOpCaps` from the selected adapter and tests
`D3DTEXOPCAPS_DOTPRODUCT3` (`0x00800000`). These callbacks occur in the
renderer dispatch table and the texture resource factory's policy view.

The isolated worker build and reccmp comparison produced these scores:

| Address | Working name | Raw similarity | Status |
| --- | --- | ---: | --- |
| `0x004db900` | `SetTextureIndexMap` | 100% | `FUNCTION` |
| `0x004dc4c0` | `ShouldUseHighResolutionTextures` | 98.11% | `FUNCTION` |
| `0x004e1f20` | `ShouldUseTextureResourceIndex` | 20.69% | `STUB` |
| `0x004e1f50` | `IsTextureResourceKindAccepted` | 32.26% | `STUB` |
| `0x004e1f80` | `IsTextureResourceKindSpecial` | 26.67% | `STUB` |
| `0x004e6380` | `SupportsDotProduct3` | 22.22% | `STUB` |

The branch conditions and tested capability bit were checked against the
complete retail disassembly. The low-scoring policy wrappers remain
unannotated as implemented functions pending sufficient raw similarity. No
runtime test was run.

The 98.11% policy is a reviewed near match: the rebuilt dispatch bridge also
zeros the unused EDX argument before the texture-quality callback. The
callback uses its ECX receiver and no stack arguments; the extra zeroing does
not change its contract, signed comparisons, or return value.
