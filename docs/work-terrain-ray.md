# Terrain ray and candidate helpers

`src/game/objects/gameobjectterrainray.cpp` contains the candidate-ray path used by the
region visibility callback. The owner view is based on concrete accesses in
the 0x004d3f00 and 0x004d3e60 bodies: level-zero cells at +0x68, grid shift at
+0xa8, level-entry storage at +0xac, grid dimensions at +0xd0/+0xd4, inverse
cell scales at +0xe0/+0xe4, candidate tile entries/count at +0xf8/+0xfc, and
output pointer/count at +0x100/+0x104. Each candidate entry has a 0x10-byte
stride; its first two signed DWORDs are column and row. No total owner size is
claimed.

The candidate callback at 0x004d3f00 sets the owner, terrain and matching-cell
globals, clears the output count, tests each candidate through the existing
terrain context callback, then traverses four rays from the scaled view point
to the candidate cell corners. A cell is appended when its top height is below
the smallest terrain sample found by the traversal. The ray callback at
0x004d3e60 either updates that minimum from a qualifying tile or terminates
when the per-cell candidate code changes.

The ray walker at 0x004f88f0 rounds start coordinates to cell centers under
the current x87 rounding mode, clips the segment to the grid, and visits cells
in boundary-crossing order. Its clipping helpers are 0x004f8ab0 and
0x004f8c40. All five recovered functions remain `STUB`: the source follows the
observed branches and layouts, but compiler output and edge-case equivalence
have not been established. Their retail extents including `ret` are 156,
406, 436, 389, and 337 bytes respectively (4d3e60, 4d3f00, 4f88f0, 4f8ab0,
4f8c40). The isolated VC6 build compiled this unit; its minimal link failed
only because the private manifest omitted renderer definitions used by the
existing terrain sources.
