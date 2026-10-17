# Renderer adapter and mode selection research

This note covers retail methods `0x004ad2c0` and `0x004ada60`, plus the small
helpers they call. The evidence is the supplied executable's full disassembly
in `data/retail-disasm.txt`, the discovered-function inventory, and the Wine
DirectX 8 headers used through [`util/directx8.h`](../util/directx8.h). There
are no Ghidra C exports for these addresses in `data/ghidra-export`.

## Supported adapter catalog

`0x004ad2c0` is a `thiscall` method with no explicit arguments, length 639
bytes (`0x004ad2c0` through `0x004ad53e`). A descriptive working name is
`EnumerateSupportedAdapters`. It reads the `IDirect3D8*` at `this+0x7ac`,
gets the adapter count through vtable slot `+0x10`, and resets the accepted
count at `this+0x38d4`. For each adapter it clears a `D3DCAPS8` (`0xd4` bytes)
and a `D3DADAPTER_IDENTIFIER8` (`0x42c` bytes), then calls
`GetDeviceCaps(adapter, D3DDEVTYPE_HAL, &caps)` through slot `+0x34` and
`GetAdapterIdentifier(adapter, D3DENUM_NO_WHQL_LEVEL, &identifier)` through
slot `+0x14`. The HRESULT from each call is stored in global
`0x00653078`; a failed call skips that adapter.

An adapter is accepted only when all tested capability bits are present:

| `D3DCAPS8` member | Offset | Required mask |
| --- | ---: | ---: |
| `DevCaps` | `0x1c` | `0x00080400` |
| `ShadeCaps` | `0x38` | `0x00004008` |
| `TextureOpCaps` | `0x90` | `0x00000044` |
| `PrimitiveMiscCaps` | `0x20` | `0x00000070` |
| `ZCmpCaps` | `0x28` | `0x00000088` |
| `SrcBlendCaps` | `0x2c` | `0x00000013` |
| `DestBlendCaps` | `0x30` | `0x0000002f` |
| `AlphaCmpCaps` | `0x34` | `0x00000088` |
| `TextureCaps` | `0x3c` | `0x00004005` |
| `TextureAddressCaps` | `0x4c` | `0x00000005` |

The first mask is the union of `D3DDEVCAPS_HWRASTERIZATION` and
`D3DDEVCAPS_DRAWPRIMTLVERTEX`. Each check requires the full mask, not merely
one bit. The routine retains at most ten accepted entries while still
iterating over the full adapter count. The catalog begins at `this+0x864`;
each `0x4d8`-byte record contains the adapter ordinal at `+0x000`, a 512-byte
driver string at `+0x004`, a 512-byte description at `+0x204`, and a copied
`D3DCAPS8` at `+0x404`. It copies the identifier strings and full capability
structure from the stack temporaries. The method returns `1` even when no
adapter is accepted; its caller separately checks `this+0x38d4`.

## Mode selection

`0x004ada60` is a `thiscall` method with four pointer arguments and
`ret 0x10`, length 307 bytes (`0x004ada60` through `0x004adb92`). A
descriptive working name is `FindBestDisplayMode`. In argument order, the
pointers refer to requested/max width, requested/max height, required bit
depth, and optional refresh rate. It uses the currently selected adapter
record pointer at `this+0x820`; its first word is the adapter ordinal.

It calls `GetAdapterModeCount(adapter)` via `IDirect3D8` vtable slot `+0x18`
and enumerates each index through `EnumAdapterModes(adapter, index, &mode)` at
slot `+0x1c`. The temporary is a zeroed `D3DDISPLAYMODE` (`0x10` bytes).
Candidates must have the helper-derived bit depth requested by the caller,
width and height no greater than the supplied maxima, and (when nonzero) an
exact refresh-rate match. It scores a candidate as
`requestedWidth + requestedHeight - mode.Width - mode.Height`; the smallest
score wins. A tie is rejected by the strict comparison against the current
best score, so the earlier enumerated mode remains selected. If there are no
modes or no matching candidate it returns `0` and does not write the output
width/height.

On success it writes the selected width and height to the first two output
pointers. It stores selected width at `this+0x7f8`, height at `+0x7fc`,
refresh rate at `+0x800`, the raw `D3DFORMAT` at `+0x804`, and requested bit
depth at `+0x808`. The raw format is retained rather than replacing it with
the helper-derived bit depth. The return value is `1`.

## Helpers and ABI

The real DirectX 8 interfaces from `d3d8.h` match the retail virtual calls:
`IDirect3D8::GetAdapterCount` is at vtable offset `+0x10`,
`GetAdapterIdentifier` at `+0x14`, `GetAdapterModeCount` at `+0x18`,
`EnumAdapterModes` at `+0x1c`, `GetAdapterDisplayMode` at `+0x20`,
`GetDeviceCaps` at `+0x34`, and `CreateDevice` at `+0x40`. The executable
pushes the interface pointer as the COM `This` argument. No replacement COM
interface or vtable is needed.

The disassembly's stack sizes confirm SDK layouts: `D3DCAPS8` is `0xd4`
bytes, `D3DADAPTER_IDENTIFIER8` is `0x42c`, and `D3DDISPLAYMODE` is `0x10`.
Their member offsets and declarations are present in the installed Wine DX8
headers; `util/directx8.h` bridges those declarations to the pinned VC6
headers.

`0x004ad540` is a 53-byte stdcall helper (`ret 4`) that maps a format value
through a small table to `16`, `24`, or `32` bits per pixel. Its default for
formats outside the table is zero. Its exact semantic name can be
`GetDisplayFormatBitDepth`; the callers pass the `D3DFORMAT` value.
`0x004ad5a0` is a 22-byte `thiscall` wrapper returning mode count for the
currently selected adapter. `0x004ad5c0` is a 124-byte `thiscall` method with
five explicit arguments (`ret 0x14`): mode index followed by output pointers
for width, height, helper-derived bit depth, and refresh rate. These
two helpers use the same `IDirect3D8*` at `this+0x7ac` and selected adapter
record at `this+0x820`.

## Owner and lifecycle evidence

The object is no longer unidentified. The PE's `.data` virtual range includes
static storage at `0x0058b140`. Startup wrapper `0x004ab450` sets ECX to that
address and jumps to `0x004ab9e0`, a 967-byte constructor spanning
`0x004ab9e0` through `0x004abda6`. The constructor installs vptr
`0x00535554`; the pointer table there begins with `0x004ac130` and
`0x004ad060`, confirming the initializer and 198-byte destructor (ending at
`0x004ad125`) as the first two virtual entries. Wrapper `0x004ab440`
invokes the constructor wrapper and then registers cleanup through
`0x004ab460`. The owner is static storage, not an object allocated by the
display initializer.

The constructor's final observed loop writes through `this+0xc7f2b`, so the
object is at least `0xc7f2c` bytes. That is a minimum footprint only: no exact
total class size or final storage boundary has been proven, and the constructor
initializes many other large renderer subsystems. The adapter/mode methods
read through `this+0x38d7`, including the actual `IDirect3D8*`, selection
state, inline ten-record catalog, and accepted count described above. The
shared `DisplayRenderer` prefix in `LEGO Racers 2/include/renderer.h` now also
models verified depth/stencil and hardware fields through `+0xc7ec7`, with a
`0xc7ec8` prefix-size assertion. This remains shorter than the constructor's
proven minimum object extent and is not a substitute for full construction.

The current source group uses `IDirect3D8`, `D3DCAPS8`,
`D3DADAPTER_IDENTIFIER8`, and `D3DDISPLAYMODE` from the existing DirectX 8
headers. The methods remain `STUB`; current integrated measurements are listed
in [progress](progress.md), and source/runtime integration is still under
review. No game run has reached display initialization.

This is a compilable adapter/mode group, but not a display-startup milestone.
The display initializer `0x004ac130` is 3,013 bytes (`0x004ac130` through
`0x004accf4`); it performs mode/configuration selection, creates the D3D
interface, and continues into device creation, render-state setup, and calls
to other subsystems. Its observed `Direct3DCreate8` argument is literal
`0x78`; do not substitute the SDK macro without separate evidence. Nearby
`0x004acd00` (193 bytes) checks the selected adapter identifier and toggles a
field at `this+0xc7ec4`, while `0x004acfb0` (169 bytes) applies render state
through the device at `this+0x7b0`. Completing the constructor and display
initializer remains necessary before startup can use this object safely.
