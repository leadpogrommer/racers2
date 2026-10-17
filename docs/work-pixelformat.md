# Pixel format description

The formats handled by `PixelFormatDescription::Initialize` are recovered from
the retail switch at `0x004ae660` (719 bytes, through its `ret 4` at
`0x004ae92c`). It has one explicit `thiscall` format parameter and writes the
mask fields at `+0x08..+0x14` and signed bit depth at `+0x18`. Ten uncompressed
RGB layouts are assigned explicit channel masks. The recognized unsupported
formats clear all four masks and set bit depth to zero. Any other enum leaves
the masks and bit depth unchanged, then continues through the common
shift/loss/bytes-per-pixel calculation; this makes the method stateful across
calls for unknown formats.

The common path resets all four shift bytes. RGB trailing-zero counts are
computed only if all three RGB masks are nonzero; alpha's shift is computed
independently when its mask is nonzero. The retail loops use arithmetic right
shifts on 32-bit masks. The normalized mask is then logically shifted by its
stored shift count. `0x004ae9d0` maps the exact normalized masks `1, 3, 7, 0xf,
0x1f, 0x3f, 0x7f` to channel losses `7..1`, and returns zero for all other
values. Finally, bytes per pixel is the signed arithmetic expression
`bitsPerPixel >> 3`, including zero or stale bit depths.

`0x004ae9d0` is a 70-byte cdecl leaf ending in `ret` at `0x004aea15`. The
isolated worker build measured `Initialize` at 69.44% raw similarity and the
loss helper at 11.96%; both remain `STUB`. The switch and return-table code
generation differs substantially from the retail tables, and no further
matching polish was attempted. The class layout is the 0x20-byte structure
supplied by the existing header; this batch does not modify that shared
declaration. No game or behavioral tests were run.
