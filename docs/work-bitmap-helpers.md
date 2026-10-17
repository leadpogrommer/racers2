# Bitmap scan helper

`FindFirstNonemptyRow` at `0x004c3460` is a 219-byte cdecl helper used by
font bitmap construction at `0x004c3540`. The four calls at `0x004c3858`,
`0x004c3888`, `0x004c38f9`, and `0x004c3931` scan in different directions
when finding glyph bounds. The complete retail body ends at the return at
`0x004c353a`; its early successful returns belong to the same function.

The six arguments are bits per pixel, a pixel pointer, signed row stride,
row count, signed pixel stride, and column count. Strides count pixels:
the 32-bit branch multiplies both by four. An 8-bit pixel is empty when its
palette index is 255; a 32-bit pixel is empty when its fourth byte is zero.
The result is the zero-based first row containing a nonempty pixel, or -1
when none is found, the format is unsupported, or the scan dimensions
prevent finding a pixel. Negative strides are used by the font caller.

The natural switch over formats 8 and 32, with post-decrement row/column
loops, matches 100% under the existing SP5 flags. An initial if/else source
form measured 59.14% because it laid out the 8-bit branch first and changed
register allocation. No compiler-flag changes or behavioral tests were used.
The current pinned-CRT candidate scan has no match for this function; engine
or third-party ownership remains unknown.
