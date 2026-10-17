# Renderer texture maintenance

This unit recovers three texture-maintenance methods from the retail renderer.
The annotations remain `STUB` until the combined build measures them.

`RendererTextureRecord::DropTopMip` (`0x004d9610`, 835 bytes) returns if the
record is already marked as processed or has fewer than two stored mip levels.
Otherwise it halves the signed dimensions, removes one mip-count bitfield
unit, sets the processed flag, recreates the primary texture through the
reserved renderer instance, and copies each old level `n + 1` into new level
`n`. Cube records repeat that copy for all six faces. The old primary texture
is released after the copy. The record's flag bit `0x10` supplies the
compression preference to the primary-texture creator.

`DisplayRenderer::RefreshTextureFormats` (`0x004dbbe0`, 943 bytes) runs only
when renderer field `+0x4dc` equals one. It scans the 800 records, selecting
active records whose flag callback at slot +0x18 returns true. A mismatch in format-selection
bits releases both texture pointers, copies the selected source record's
format bits, recreates the primary texture, and recreates the secondary
texture only when one existed before release. It then copies matching mip
surfaces from the current display-format source record; cube records use the
six cached source record IDs. Existing secondary textures are updated after
the copies.

`RendererTextureRecord::ReleaseTexture` (`0x004dbfb0`, 77 bytes) accepts slot
zero or one, releases the pointer at `+0x94` or `+0x98` if non-null, then clears
that slot. The retail branch on flag bit `0x08` has the same release-and-clear
effects in both arms.

The two surface-copy algorithms are distinct. DropTopMip checks both locks,
copies sourcePitch bytes per descending row, and unlocks only successfully
locked surfaces. It does not compare surface descriptors. Format refresh
ignores lock results, compares GetDesc Size fields and copies the entire byte
size when equal, then unlocks and releases both surfaces. Its source cube faces
are separate 2D textures; only the destination uses GetCubeMapSurface. The root
review corrected the original attempt to share one row-copy helper for both
paths and an erroneous call to Load instead of the +0x18 flag query.

Function extents are based on complete terminal return instructions:

| Address | Length | Recovered signature |
| --- | ---: | --- |
| `0x004d9610` | 835 | `void RendererTextureRecord::DropTopMip()` |
| `0x004dbbe0` | 943 | `void DisplayRenderer::RefreshTextureFormats()` |
| `0x004dbfb0` | 77 | `void RendererTextureRecord::ReleaseTexture(LegoU32 slot)` |

No runtime or game-function behavioral tests were run. An isolated SP5 build is
used for compile/link validation; root owns shared manifests and metadata.
