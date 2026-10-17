# Bitmap font glyph metrics

`BitmapFont::GetGlyphMetrics` at `0x004c3db0` is 122 bytes, through its
`ret 0xc` at `0x004c3e27`. It matches 100% with the pinned SP5 toolchain.
The caller at `0x004b4273` supplies a character, an output quad and an advance
pointer, and compares the integer result with 1. The callee masks the character
to 16 bits and uses the byte lookup table at font +0x1c30 to select a glyph.
The model preserves that indexing without adding a bounds check.

The loader at `0x004c3540`, which also uses the recovered bitmap scanner,
establishes 28-byte glyph records starting at +0x30. Each holds a texture
identifier, normalized u/v origin, pixel width/height, and normalized u/v
extents. Its loop advances by 0x1c. The 256-entry byte character map is copied
at +0x1c30. Glyph UV extents come from `(dimension - 1) / texture dimension`;
the origin includes the loader's half-pixel adjustment. The record stride
supports the glyph size assertion; no complete font or quad size is claimed.

The helper copies texture and UV fields into output +0x2c through +0x3c,
forms the final UV corners, computes horizontal advance as
`(spacing + glyph width) * scaleX`, and fills right/bottom coordinates from
the existing left/top and scaled glyph dimensions. Font scale fields are
at +0x14/+0x18 and spacing at +0x2c. Other font and output members remain
gaps. The surrounding loader, resource ownership and rendering paths remain
unimplemented; their interfaces are not needed for this leaf helper.

This is a font-table operation grounded in the loader and text caller, rather
than a generic pixel or CRT helper. Names are descriptive recovered names,
not original debug symbols.

## Image constructor used by the loader

`BitmapImage::BitmapImage` at `0x004c3d70` is 49 bytes, ending with `ret`
at `0x004c3da0`. Its member-initializer implementation matches 100%. The font
loader constructs the image at stack offset -0x30, passes it to the image
loader at `0x004f54a0`, reads signed 16-bit width/height at +8/+0xa, and uses
the pixel storage for its glyph-bound scans. The image loader uses +0xc as
bits per pixel when calculating buffer sizes. Two pointer members at +0/+4
are released by `0x004f59c0`. The loader reads color-map entries into +0 and
pixel data into +4; the font scanner receives the +4 pointer. Byte +0xd is
the mip-level count: image storage calculations sum width/height shifted by
each level. Width/height remain signed 16-bit fields as observed by callers.

The constructor zeros both pointers, both dimensions and bits per pixel,
sets the mip-level count to 1 and +0x10 to -1, and updates packed fields at
+0xe/+0xf. The low nine bits hold a palette-entry count initialized to 256;
the loader masks with 0x1ff and multiplies by the color-map entry width before
reading the palette. Flag bits 9 and 10 are initialized to 0 and 1. The upper
five bits remain untouched. Modeling the count and flags as 16-bit bitfields
reproduces the read/modify/write instructions without clearing reserved bits.
The two flag meanings and complete image extent remain unknown. Image loading
and cleanup still depend on the deferred memory-manager interface, so no
guessed loader/destructor declarations were added.
