# Renderer hardware policy

`DisplayRenderer::ConfigureNvidiaWorkaround` recovers the complete control flow
of retail `0x004acd00`, called directly by the display initializer at
`0x004ac8ed`. Its code extent is 193 bytes, through `0x004acdc0`; the following
embedded switch tables are data. The name describes an inferred hardware
workaround, not an identified original symbol or a proven specific driver bug.

The method clears the flag at `this+0xc7ec4`, then calls the real DirectX 8
`GetAdapterIdentifier` with the ordinal at **`this+0x7b4`**, flag
`D3DENUM_NO_WHQL_LEVEL`, and an uninitialized identifier output. It only reads
the identifier after a zero HRESULT. This uses the requested adapter ordinal,
not the selected-adapter record pointer at `+0x820`.

For vendor `0x10de`, the accepted device IDs are `0x20`, `0x28`, `0x29`,
`0x2c`, `0x2d`, `0xa0`, `0x100`, `0x101`, `0x103`, `0x110`, `0x111`,
`0x113`, `0x150`, `0x151`, `0x152`, `0x153`, and `0x200`. These were decoded
from both switch tables in the hash-verified retail PE: byte table
`0x004acdcc` covers `0x20..0x101`, the explicit equality covers `0x103`, and
byte table `0x004aceb8` covers `0x110..0x200`. The zero-valued entries select
the flag-setting branch; all other device IDs leave it clear.

For accepted hardware, `this+0x2456c` is cleared **before** applying the
configuration's `NVidia` gate at `+0x2c`. A zero configuration value then
clears the workaround flag again, but does not undo that earlier write. A
failed identifier query or different vendor leaves `+0x2456c` untouched.
The purpose of this latter field remains unknown.

The renderer header now describes one common observed prefix for hardware,
adapter, mode and depth/stencil methods. Its size assertion ends at `0xc7ec8`;
the original constructor writes through `+0xc7f2b`, so this is explicitly not
the complete owner or a substitute for construction. No window or renderer
device is created by these helpers. The method measures 12.61% raw instruction
accuracy in both combined VC6 builds and remains `STUB`. An independent review
of the hash-verified PE confirmed the device set, requested-adapter ABI,
failure paths and configuration-gate ordering. The low score does not qualify
for the implemented-function threshold; matching polish was not allowed to
delay the next startup dependencies.
