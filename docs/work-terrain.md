# Terrain bounds, tiles and vertex lookup

The routines below form a closed heightfield lookup path. Names are recovered
descriptions, not original debug symbols. Their surrounding rendering,
resource ownership and terrain construction remain unrecovered.

| Original VA | Bytes | Function | Instruction match |
| --- | ---: | --- | ---: |
| 0x004c3f40 | 506 | Terrain::GetCellVertices | STUB, 63.61% |
| 0x004c4140 | 85 | Terrain::GetMinimumBounds | 100% |
| 0x004c41a0 | 81 | Terrain::GetMaximumBounds | 100% |
| 0x004cb340 | 289 | TerrainVertexGrid::SelectEdges | 100% |
| 0x004cb630 | 68 | Terrain::GetVertex | 100% |
| 0x004cb680 | 66 | Terrain::GetTile | 100% |

The five exact results use the pinned SP5 compiler and existing flags. Natural
loop increments in the border selector and local coordinates in GetVertex
resolved their initial instruction scheduling/register differences. The cell
helper retains STUB status. No behavior tests or compiler-flag changes were
introduced. Focused pinned-CRT scans produced no candidates for these
six helpers; the tile/heightfield accesses and callers ground their role, without
establishing original engine or third-party authorship.

## Layout and bounds ABI

The partial Terrain model has axis scales at +0x138/+0x13c/+0x140, column/row
counts at +0x15c/+0x160, floating coordinate origins at +0x164/+0x168,
integer minimum/maximum heights at +0x16c/+0x170, tile counts at
+0x1bc/+0x1c0, cell-span limits at +0x1c8/+0x1cc, and a tile pointer at
+0x1d0. Unused fields remain gaps; the complete Terrain extent is unknown.

Bounds use integer `(dimension + 1) / 2`, including signed rounding toward
zero, before conversion and scaling. Minimum bounds negate the horizontal
half-extents; vertical bounds are the corresponding integer height times
the vertical scale. The retail instructions calculate z, y, x before storing
x, y, z, which also matters for overlapping output storage.

Callers at 0x0042d369/0x0042d389 and 0x00495e34/0x00495e5a pass a three-float
temporary and consume its address in EAX; each callee returns with `ret 4`.
The source uses an explicit Vector3 output pointer and returns that pointer.
A source-level vector return with a hidden result pointer could express the
same ABI; the original C++ spelling and const qualification remain unknown.

## Tiles and border selection

GetTile performs signed divisions and indexes tiles in column-major order:
`(tileColumns * column / columns) * tileRows + tileRows * row / rows`.
It performs no bounds checks. The final address calculation establishes the
0x120 tile stride. Each tile stores signed 16-bit column/row origins at
+0xc/+0xe and a vertex-grid prefix at +0x90. No complete grid size is asserted.

The grid has a vertex pointer at +0, signed 16-bit column/row counts at
+4/+6, a 16-bit edge-height buffer pointer at +8, two sets of four signed
16-bit buffer offsets at +0xc/+0x14, and four cached selection bytes at +0x1c.
Vertex indexing establishes an 8-byte record stride. Its first word is an
unsigned height, and byte +5 supplies the cell helper's flag bit; remaining
bytes are unknown.

SelectEdges compares the four-byte selection as a word and, for each changed
edge, chooses an offset from set zero or one according to whether that edge's
selection byte is zero. It copies heights in left, right, top, bottom order
and updates each cached byte before copying. That order preserves the original
corner overwrites. It is CPU-side height data handling, with no SDK interface
calls. The union models both the packed comparison and individual bytes.

GetVertex selects a tile, subtracts its integer origins, updates its borders
using the state at global 0x0067b68c, and returns the corresponding vertex.
The state at 0x0067a688 serves the cell helper's direct-grid path. Both are
four-byte BSS globals; their initial zero values pass data comparison.

## Cell reconstruction

GetCellVertices rejects negative coordinates and the last/outside grid row
or column before writing outputs. A cell crossing a tile-span boundary uses
four GetVertex calls; an interior cell updates the selected tile's borders
and obtains all four pointers directly. It computes the four world-space
vertices in `(column,row)`, `(column,row+1)`, `(column+1,row)`,
`(column+1,row+1)` order, scaling unsigned heights for y. Its final result is
the complement of bit zero in the last vertex's flag byte. That flag check
happens after output writes, so FALSE does not always mean outputs were left
untouched. The reconstruction preserves those branches and store order;
remaining stack/register differences are visible in the assembly report.

The combined Python/Wine and CMake/NMake builds agree on all six scores above.
The new global variables pass data comparison in both builds. All function
extents were decoded through their final returns; the heightfield cell
helper includes its early-return branches as well as the final `ret 0xc`.
