#ifndef LR2_TERRAINVERTEXGRID_H
#define LR2_TERRAINVERTEXGRID_H

#include "decomp.h"
#include "types.h"

struct TerrainVertex {
	LegoU16 m_height; // 0x00
	LegoU8 m_unk0x02[5 - 2];
	LegoU8 m_flags; // 0x05
	LegoU8 m_unk0x06[8 - 6];
};

DECOMP_SIZE_ASSERT(TerrainVertex, 8);

union TerrainEdgeSelection {
	LegoU32 m_packed;
	LegoU8 m_edges[4]; // left, top, right, bottom
};

class TerrainVertexGrid {
public:
	void SelectEdges(const TerrainEdgeSelection* p_selection);

	TerrainVertex* m_vertices;        // 0x00
	LegoS16 m_columns;                // 0x04
	LegoS16 m_rows;                   // 0x06
	const LegoU16* m_edgeHeights;     // 0x08
	LegoS16 m_edgeOffsets[2][4];      // 0x0c, 0x14
	TerrainEdgeSelection m_selection; // 0x1c
};

#endif // LR2_TERRAINVERTEXGRID_H
