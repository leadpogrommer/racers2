# Renderer texture record creation

This batch adds `DisplayRenderer::CreateTextureRecord` and the record's
primary-surface lock/unlock callbacks to `src/renderertexturefactory.cpp`.
Their raw reccmp results from the isolated VC6/DX8 build are:

| Address | Length | Raw match | Status |
| --- | ---: | ---: | --- |
| `0x004d9f20` | 245 (`0x0f5`) | 79.01% | STUB |
| `0x004da490` | 107 (`0x06b`) | 57.89% | STUB |
| `0x004da500` | 60 (`0x03c`) | 80.85% | STUB |

The factory is a `thiscall` method with seven stack arguments. Retail uses the
second argument as the texture-record index, forwards arguments three through
five as width, height and mip count, writes the sixth argument as an output
flag, and forwards the seventh as `SetTextureRecordDimensions`'s `flagA`. The
first argument selects one of three format modes. It first dispatches the
texture-index map callback with the record index as both values, configures the
record, creates its primary texture, and conditionally creates a secondary
device texture when the renderer's `+0xc7ed4` flag is set.

On primary-texture creation failure, retail calls record vtable slot `+0x0c`
before clearing the record's in-use bit. The implementation retains this real
dispatch rather than replacing it with a guessed cleanup body. That target is
`0x004c0050`; it forwards the signed record ID to the application dispatch at
`+0xe4`, whose target remains part of the application resource-release
closure.

The `0x004da490` callback locks primary texture level zero, returns the locked
pitch and pixel pointer through its first and third arguments, and releases the
surface reference while leaving the texture surface locked. Its second stack
argument is unused. `0x004da500` reacquires level zero, unlocks it, releases the
surface, then updates the GPU texture. The D3D8 calls use the real SDK
interfaces; no replacement COM vtable is introduced.

These bodies remain STUB because their raw matches are below the project's
near-match threshold. An isolated full-source build and per-function reccmp
comparisons passed; no runtime or behavioral tests were run.
