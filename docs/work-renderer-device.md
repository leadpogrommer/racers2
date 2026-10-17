# Renderer device setup research

This pass follows the renderer initializer at `0x004ac130` and implements its
small depth/stencil-format selector. The evidence is the full retail listing
in `data/retail-disasm.txt`, the DirectX 8 interfaces in `util/directx8.h`, and
the current partial renderer layout in `LEGO Racers 2/include/renderer.h`.

## Initialization order and current boundary

`0x004ac130` is a 3,013-byte (`0x0bc5`) `thiscall` initializer ending at
`0x004accf4`. The constructor-owned object is static at `0x0058b140`; the
constructor writes vptr `0x00535554`, whose first two entries are the
initializer `0x004ac130` and destructor `0x004ad060`. The initializer is not
the owner constructor.

The observed startup chain is:

1. Read the application display tuple and allocate a `0x3c`-byte
   `DisplayConfiguration` through `0x004a4190`. There is no separate
   configuration constructor on this path: `0x004ac1c7–0x004ac1df` writes
   defaults at offsets `0x10–0x38`, then publishes the pointer at `0x0065307c`.
2. Resolve `/options`, `/windowed`, `/hz`, `/bpp`, `/device`, and
   `/wireframe` through `0x004b4a60` and `0x004b4b30`; those command-line
   helpers are reconstructed elsewhere. The application virtual calls supply
   the default display tuple and fullscreen policy.
3. Create the native window through `0x004b46c0`, passing the window procedure
   stored at `0x0067a4ec`.
4. Call `Direct3DCreate8(0x78)`, enumerate supported adapters with
   `0x004ad2c0`, choose an adapter and display mode with `0x004ada60`, then
   build presentation parameters. `0x004aebb0` is called immediately before
   the first `IDirect3D8::CreateDevice` call.
5. Create and configure the device, retry selected fullscreen/windowed modes
   on failure, initialize render targets and other renderer resources, then
   continue into later renderer and application callbacks.

The path is still not a closed startup implementation. The initializer uses
the renderer object's vtable and fields beyond the shared typed prefix. The
constructor proves a minimum owner extent of `0xc7f2c` bytes, while the common
`DisplayRenderer` model currently reaches `0xc7efc`; neither is a full class
size proof. The initializer depends on the renderer lifecycle/layout,
application configuration dispatch, a native window at the right lifecycle
point, and many renderer/resource methods. In particular, it calls the large
reset/recreation path at `0x004ad640` and later uses additional virtual slots
and helper groups after device creation. The typed prefix is not a substitute
for complete `DisplayRenderer` construction.

The adapter and mode-selection helpers have confirmed real DirectX 8
contracts. `0x004ad2c0` enumerates capabilities and records up to ten accepted
adapters; `0x004ada60` searches display modes under caller-supplied width,
height, bit-depth and refresh constraints. The initializer uses
`IDirect3D8::GetAdapterDisplayMode` at vtable offset `0x20` and
`IDirect3D8::CreateDevice` at offset `0x3c`. The `Direct3DCreate8` version
argument is the literal `0x78` in retail and should remain that value.

## Depth/stencil-format selection

`0x004aebb0` is a 163-byte (`0xa3`) `thiscall` method with no explicit
arguments. Its six candidate formats and their order are visible at
`0x00549e00` and `0x00549e1c`:

| Requested bit depth | Candidate order |
| --- | --- |
| `<= 16` | `D3DFMT_D16, D3DFMT_D15S1, D3DFMT_D24S8, D3DFMT_D24X4S4, D3DFMT_D32, D3DFMT_D24X8` |
| `> 16` | `D3DFMT_D24S8, D3DFMT_D24X4S4, D3DFMT_D32, D3DFMT_D24X8, D3DFMT_D16, D3DFMT_D15S1` |

The routine clears DWORDs at `this+0x24570` and `this+0x24574`, then calls
`IDirect3D8::CheckDeviceFormat` for candidates using the selected adapter
ordinal at `this+0x820`, HAL device type, display format at `this+0x804`,
`D3DUSAGE_DEPTHSTENCIL`, and `D3DRTYPE_SURFACE`. The first successful
candidate is saved at `this+0x7dc`; the DWORD at `this+0x24570` becomes one
only when the selected format equals `D3DFMT_D24S8` (`0x4b`). The requested
bit-depth comparison is signed. It returns one when a candidate works and
zero when all six checks fail.

This method is implemented as `DisplayRenderer::SelectDepthStencilFormat` in
`LEGO Racers 2/src/rendererdevice.cpp` and shares the typed prefix in
`renderer.h`. The prefix names only fields supported by the retail accesses;
its size assertion ends at `0xc7efc` and does not claim the full renderer size.
It aliases the existing static renderer allocation; it is not constructed
separately. The combined Visual C++ 6 SP5 builds passed with the generated
DirectX 8 configuration. The function remains `STUB` at its measured 60.38%
raw score. No game or probe was run for this implementation batch.

## Next closed unit

The smallest next prerequisite is the renderer presentation-parameter and
device-creation sequence inside `0x004ac130`, followed by the complete reset
path at `0x004ad640`. That work needs the rest of the presentation/device
fields and the application getter contracts already called by the initializer.
Only after the device exists can the small render-target allocator at
`0x004acfb0` be wired meaningfully; it creates vertex buffers on the device at
`this+0x7b0` and consumes state well beyond the existing adapter prefix.
Until those fields and callees are closed, the adapter catalog and mode
selection are prerequisites rather than a running renderer. The presentation
block at `+0x7b8` uses the real `D3DPRESENT_PARAMETERS` type, whose `0x34`-byte
size is asserted against the DirectX 8 declaration.
