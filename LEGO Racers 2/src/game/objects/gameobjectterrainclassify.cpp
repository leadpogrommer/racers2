#include "game/objects/gameobjectterrainvisibility.h"
#include "math/vector3.h"

#include <stddef.h>
#include <stdlib.h>

namespace
{

struct TerrainCellRecordView {
	Vector3 m_center;
	LegoFloat m_cullingRadius;
	LegoFloat m_halfExtentX;
	LegoFloat m_halfExtentY;
	LegoFloat m_halfExtentZ;
	LegoFloat m_unk0x1c;
};

struct TerrainCellOwnerView {
	LegoU8 m_opaque0x00[0x108];
	Vector3 m_planeNormals[5];
	Vector3 m_planePoints[5];
	LegoU8 m_opaque0x180[0x18];
	LegoU32 m_planeEnd;
	LegoU32 m_planeCount;
};

struct TerrainCellGridView {
	LegoU8 m_opaque0x00[0x68];
	void* m_levelRecords[16];
	LegoU32 m_gridShift;
};

DECOMP_STATIC_ASSERT(offsetof(TerrainCellRecordView, m_cullingRadius) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(TerrainCellRecordView, m_halfExtentX) == 0x10);
DECOMP_STATIC_ASSERT(offsetof(TerrainCellOwnerView, m_planeNormals) == 0x108);
DECOMP_STATIC_ASSERT(offsetof(TerrainCellOwnerView, m_planePoints) == 0x144);
DECOMP_STATIC_ASSERT(offsetof(TerrainCellOwnerView, m_planeEnd) == 0x198);
DECOMP_STATIC_ASSERT(offsetof(TerrainCellOwnerView, m_planeCount) == 0x19c);
DECOMP_STATIC_ASSERT(offsetof(TerrainCellGridView, m_gridShift) == 0xa8);

static LegoFloat DotFromPlanePoint(const Vector3& p_point, const Vector3& p_normal, const Vector3& p_planePoint)
{
	return (p_point.m_x - p_planePoint.m_x) * p_normal.m_x + (p_point.m_y - p_planePoint.m_y) * p_normal.m_y +
		   (p_point.m_z - p_planePoint.m_z) * p_normal.m_z;
}

static void __cdecl TerrainVisibilityCleanup()
{
}

} // namespace

// GLOBAL: LEGORACERS2 0x0067b860
LegoU8 g_terrainVisibilityCleanupRegistered;

// GLOBAL: LEGORACERS2 0x0067ba80
Vector3 g_terrainCellCornerSamples[8];

// GLOBAL: LEGORACERS2 0x0067ba74
Vector3* g_terrainVisibilitySampleCursor;

// GLOBAL: LEGORACERS2 0x0067ba78
LegoU32 g_terrainVisibilitySamplePlaneMask;

// GLOBAL: LEGORACERS2 0x0067bae8
LegoU32 g_terrainVisibilityUnionPlaneMask;

// GLOBAL: LEGORACERS2 0x0067b850
LegoU32 g_terrainVisibilityCommonPlaneMask;

// STUB: LEGORACERS2 0x004d4520
LegoU32 __fastcall ClassifyTerrainRegionCell(void* p_owner, void*, void* p_cell)
{
	TerrainCellOwnerView* owner = (TerrainCellOwnerView*) p_owner;
	TerrainCellRecordView* cell = (TerrainCellRecordView*) p_cell;
	LegoFloat negativeRadius = -cell->m_cullingRadius;

	for (LegoU32 plane = 0; (const LegoU8*) owner + 0x108 + plane * sizeof(Vector3) < (const LegoU8*) owner->m_planeEnd;
		 ++plane) {
		if (DotFromPlanePoint(cell->m_center, owner->m_planeNormals[plane], owner->m_planePoints[plane]) <=
			negativeRadius) {
			return 4;
		}
	}

	if ((g_terrainVisibilityCleanupRegistered & 1) == 0) {
		g_terrainVisibilityCleanupRegistered |= 1;
		atexit(TerrainVisibilityCleanup);
	}

	g_terrainVisibilityUnionPlaneMask = 0;
	g_terrainVisibilityCommonPlaneMask = 0xffffffff;
	BuildTerrainCellCorners(cell, NULL, g_terrainCellCornerSamples);

	for (LegoU32 sample = 0; sample < 8; ++sample) {
		Vector3* point = &g_terrainCellCornerSamples[sample];
		LegoU32 planeMask = 0;
		for (LegoU32 plane = 0; plane < owner->m_planeCount; ++plane) {
			if (DotFromPlanePoint(*point, owner->m_planeNormals[plane], owner->m_planePoints[plane]) < 0.0f) {
				planeMask = 1 << (plane & 0x1f);
				break;
			}
		}
		g_terrainVisibilitySamplePlaneMask = planeMask;
		g_terrainVisibilityUnionPlaneMask |= planeMask;
		g_terrainVisibilityCommonPlaneMask &= planeMask;
		g_terrainVisibilitySampleCursor = point + 1;
	}

	if (g_terrainVisibilityUnionPlaneMask == 0) {
		return 0;
	}
	return g_terrainVisibilityCommonPlaneMask != 0 ? 4 : 8;
}

// STUB: LEGORACERS2 0x004d48e0
void __fastcall UpdateTerrainRegionCellNeighbors(void* p_owner, void*, LegoS32 p_level, LegoS32 p_column, LegoS32 p_row)
{
	TerrainCellOwnerView* owner = (TerrainCellOwnerView*) p_owner;
	TerrainCellGridView* grid = (TerrainCellGridView*) p_owner;
	LegoU32* levelBases = (LegoU32*) ((LegoU8*) owner + 0x68);
	LegoS32 childColumn = p_column * 2;
	LegoS32 childRow = p_row * 2;
	LegoS32 levelShift = (LegoS32) grid->m_gridShift;
	LegoS32 shift = (levelShift - p_level) & 0x1f;
	LegoS32 rowStride = *(LegoS32*) ((LegoU8*) owner + 0xd0) >> (p_level & 0x1f);
	LegoS32 firstIndex = (childRow << shift) + childColumn;
	LegoU8* cell = (LegoU8*) levelBases[p_level] + firstIndex * 0x20;
	LegoS32 childLevel = p_level - 1;

	for (LegoS32 row = 0; row < 2; ++row) {
		for (LegoS32 column = 0; column < 2; ++column) {
			LegoU8* current = cell + row * rowStride * 0x20 + column * 0x20;
			LegoU32 state = ClassifyTerrainRegionCell(p_owner, NULL, current);
			*(LegoU32*) (current + 0x1c) = state;
			if (state == 8 && childLevel >= 0) {
				UpdateTerrainRegionCellNeighbors(p_owner, NULL, childLevel, childColumn + column, childRow + row);
			}
		}
	}
}

// STUB: LEGORACERS2 0x004d5cd0
void __fastcall BuildTerrainCellCorners(void* p_cell, void*, Vector3* p_points)
{
	TerrainCellRecordView* cell = (TerrainCellRecordView*) p_cell;
	LegoFloat minX = cell->m_center.m_x - cell->m_halfExtentX;
	LegoFloat minY = cell->m_center.m_y - cell->m_halfExtentY;
	LegoFloat minZ = cell->m_center.m_z - cell->m_halfExtentZ;
	LegoFloat maxX = cell->m_center.m_x + cell->m_halfExtentX;
	LegoFloat maxY = cell->m_center.m_y + cell->m_halfExtentY;
	LegoFloat maxZ = cell->m_center.m_z + cell->m_halfExtentZ;
	p_points[0].m_x = maxZ;
	p_points[0].m_y = minY;
	p_points[0].m_z = maxZ;
	p_points[1].m_x = maxX;
	p_points[1].m_y = minY;
	p_points[1].m_z = maxZ;
	p_points[2].m_x = minX;
	p_points[2].m_y = minY;
	p_points[2].m_z = minZ;
	p_points[3].m_x = maxX;
	p_points[3].m_y = minY;
	p_points[3].m_z = minZ;
	p_points[4].m_x = maxZ;
	p_points[4].m_y = maxY;
	p_points[4].m_z = maxZ;
	p_points[5].m_x = maxX;
	p_points[5].m_y = maxY;
	p_points[5].m_z = minZ;
	p_points[6].m_x = maxZ;
	p_points[6].m_y = maxY;
	p_points[6].m_z = maxZ;
	p_points[7].m_x = maxX;
	p_points[7].m_y = maxY;
	p_points[7].m_z = minZ;
}
