#ifndef LR2_TERRAIN_H
#define LR2_TERRAIN_H

#include "math/vector3.h"
#include "scene/terrain/terrainvertexgrid.h"
#include "types.h"

struct TerrainTile {
	LegoU8 m_unk0x00[0x0c];
	LegoS16 m_column; // 0x0c
	LegoS16 m_row;    // 0x0e
	LegoU8 m_unk0x10[0x90 - 0x10];
	TerrainVertexGrid m_vertexGrid; // 0x90
	LegoU8 m_unk0xb0[0x120 - 0xb0];
};

DECOMP_SIZE_ASSERT(TerrainTile, 0x120);

class Terrain {
public:
	LegoBool32 GetCellVertices(LegoS32 p_column, LegoS32 p_row, Vector3* p_vertices) const;
	Vector3* GetMinimumBounds(Vector3* p_result) const;
	Vector3* GetMaximumBounds(Vector3* p_result) const;
	TerrainVertex* GetVertex(LegoS32 p_column, LegoS32 p_row) const;
	TerrainTile* GetTile(LegoS32 p_column, LegoS32 p_row) const;
	LegoBool32 GetCellRange(
		const Vector3* p_position,
		LegoFloat p_radius,
		LegoS32* p_minColumn,
		LegoS32* p_minRow,
		LegoS32* p_maxColumn,
		LegoS32* p_maxRow
	) const;
	LegoBool32 ProjectToSurface(
		const Vector3* p_worldPosition,
		Vector3* p_surfacePosition,
		Vector3* p_worldNormal
	) const;

	LegoU8 m_unk0x00[0x138];
	Vector3 m_scale;           // 0x138
	LegoFloat m_inverseScaleX; // 0x144
	LegoFloat m_unk0x148;
	LegoFloat m_inverseScaleZ; // 0x14c
	LegoU8 m_unk0x150[0x15c - 0x150];
	LegoS32 m_columns;        // 0x15c
	LegoS32 m_rows;           // 0x160
	LegoFloat m_columnOrigin; // 0x164
	LegoFloat m_rowOrigin;    // 0x168
	LegoS32 m_minHeight;      // 0x16c
	LegoS32 m_maxHeight;      // 0x170
	LegoU8 m_unk0x174[0x1bc - 0x174];
	LegoS32 m_tileColumns; // 0x1bc
	LegoS32 m_tileRows;    // 0x1c0
	LegoU8 m_unk0x1c4[4];
	LegoS32 m_tileColumnSpan; // 0x1c8
	LegoS32 m_tileRowSpan;    // 0x1cc
	TerrainTile* m_tiles;     // 0x1d0

	// Partial layout; no complete object size is claimed.
};

#endif // LR2_TERRAIN_H
