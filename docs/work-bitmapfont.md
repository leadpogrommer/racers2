# Bitmap font loading

`BitmapFontSet` models the four font pointers stored by the game controller.
Its initializer at `0x0042cef0` loads slots 0 and 1 from the small and normal
font TGA files, then makes the remaining selected fonts by copying an existing
font. The mode-specific release at `0x0042d020` frees only selected slots;
the destructor releases all four. These units are still outside the shared
source manifest while the atlas loader is under review.

The allocation size is `0x1d54`. The loader writes the font index at +0, source
bits per pixel at +4, glyph count at +8, atlas-page dimension at +0x10, scales
at +0x14/+0x18, glyph records at +0x30 (stride `0x1c`), the byte character map
at +0x1c30, up to eight texture IDs at +0x1d30 and their count at +0x1d50.
The callers allocate exactly `0x1d54` bytes. The four profile-specific scale
and spacing adjustments are applied after loading or copying.

`BitmapFont::Load` at `0x004c3540` is implemented in the separate loader unit.
It initializes the 256-byte byte-to-glyph map, loads the source TGA, chooses a
power-of-two atlas dimension bounded by the selected adapter, scans character
cells for visible bounds, packs pixels into temporary pages, and uploads each
page through the renderer texture-record callbacks. It then stores normalized
glyph coordinates and copies the character map. The fontset callers establish
the five-stack-argument thiscall signature and pass the charset, glyph-cell
width and case mode.

The square-root result is stored to a float and converted back with x87
`FISTP`, preserving the active x87 rounding mode. Page allocation passes the
`Temp Font Bitmap` diagnostic label; temporary texture resources use an empty
path. The uploader passes the page image mode, dimensions and one mip level.
It copies only the palette-entry bits from the source image so the page
constructor's other flag bits remain intact. The retail upload-failure path
releases the source image but leaves the scratch page allocation outstanding,
and the implementation preserves the unchecked eight-entry texture list.

The loader remains `STUB` pending isolated raw comparison and final review of
the full atlas path. It uses the existing `FindFirstNonemptyRow` helper from
`bitmaputil.cpp`; no duplicate scanner is declared. Page source addressing,
last-page height normalization, map case-pair behavior and upload failure
branches have been checked against the disassembly. No game or rendering
runtime test has been run for this unit.

`BitmapFont::CopyFrom` at `0x004c3ed0` copies the full allocation, replaces its
first word with the destination resource kind, and increments references for
its copied texture IDs. `FindFirstNonemptyRow` at `0x004c3460` scans 8-bit
palette pixels (index `0xff` is transparent) and 32-bit alpha bytes (zero is
transparent), including the caller's signed strides. The font set, loader and
text rendering path remain outside the main manifest pending root integration.
