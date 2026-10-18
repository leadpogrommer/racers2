#include "gameobjectterrainvisibility.h"
#include "rendererterrain.h"
#include "terrain.h"
#include "vector3.h"

namespace
{

struct TerrainRegionCellView {
	LegoU8 m_opaque0x00[4];
	LegoFloat m_halfExtentY;
	LegoU8 m_opaque0x08[0x0c];
	LegoFloat m_centerY;
	LegoU8 m_opaque0x18[0x08];
};

struct TerrainRegionOwnerView {
	Terrain* m_terrain;
	LegoU8 m_opaque0x04[0x64];
	TerrainRegionCellView* m_levelRecords[16];
	LegoU32 m_gridShift;
	void* m_levelEntries;
	void* m_currentMap;
	void* m_nextMap;
	LegoU8 m_opaque0xb8[0x08];
	LegoU32 m_levelCount;
	LegoU8 m_opaque0xc4[0x0c];
	LegoS32 m_gridWidth;
	LegoS32 m_gridDepth;
	LegoU8 m_opaque0xd8[0x08];
	LegoFloat m_inverseCellWidth;
	LegoFloat m_inverseCellDepth;
	LegoU8 m_opaque0xe8[0x10];
	void* m_candidateTiles;
	LegoS32 m_candidateTileCount;
	void* m_candidateOutput;
	LegoS32 m_candidateOutputCount;
	LegoU8 m_opaque0x108[0x90];
	LegoU32 m_cullingPlanePointer;
	LegoU32 m_cullingPlaneCount;
	LegoU8 m_opaque0x1a0[4];
};

DECOMP_STATIC_ASSERT(offsetof(TerrainRegionOwnerView, m_levelRecords) == 0x68);
DECOMP_STATIC_ASSERT(offsetof(TerrainRegionOwnerView, m_gridShift) == 0xa8);
DECOMP_STATIC_ASSERT(offsetof(TerrainRegionOwnerView, m_levelEntries) == 0xac);
DECOMP_STATIC_ASSERT(offsetof(TerrainRegionOwnerView, m_levelCount) == 0xc0);
DECOMP_STATIC_ASSERT(offsetof(TerrainRegionOwnerView, m_gridWidth) == 0xd0);
DECOMP_STATIC_ASSERT(offsetof(TerrainRegionOwnerView, m_inverseCellWidth) == 0xe0);
DECOMP_STATIC_ASSERT(offsetof(TerrainRegionOwnerView, m_candidateTiles) == 0xf8);
DECOMP_STATIC_ASSERT(offsetof(TerrainRegionOwnerView, m_candidateOutputCount) == 0x104);
DECOMP_STATIC_ASSERT(offsetof(TerrainRegionOwnerView, m_cullingPlanePointer) == 0x198);

struct TerrainRegionTileView {
	LegoU8 m_opaque0x00[0x11e];
	LegoU8 m_typeFlags;
};

static LegoS32 RoundFloatUsingCurrentFpuMode(LegoFloat p_value)
{
	volatile LegoFloat storedValue = p_value;
	LegoS32 result;
	__asm {
		fld storedValue
		fistp result
	}
	return result;
}

static TerrainRegionTileView* GetRegionTile(Terrain* p_terrain, LegoS32 p_column, LegoS32 p_row)
{
	LegoU8* terrainBytes = (LegoU8*) p_terrain;
	LegoS32 tileRows = *(LegoS32*) (terrainBytes + 0x1c0);
	TerrainRegionTileView* tiles = *(TerrainRegionTileView**) (terrainBytes + 0x1d0);
	LegoU32 tileIndex = (LegoU32) tileRows * (LegoU32) p_column + (LegoU32) p_row;
	return &tiles[tileIndex];
}

} // namespace

LegoBool32 __cdecl ClipTerrainGridSegment(LegoFloat*, LegoFloat*, LegoFloat*, LegoFloat*, LegoFloat, LegoFloat);
LegoBool32 __cdecl ClipTerrainGridBoundary(LegoFloat, LegoFloat, LegoFloat, LegoFloat*, LegoFloat*);
LegoBool32 __cdecl IsTerrainRegionRayCell(LegoS32 p_column, LegoS32 p_row);
LegoU32 __cdecl TraverseTerrainGridRay(
	LegoFloat,
	LegoFloat,
	LegoFloat,
	LegoFloat,
	LegoS32,
	LegoS32,
	LegoBool32(__cdecl*)(LegoS32, LegoS32)
);

// GLOBAL: LEGORACERS2 0x0067b854
void* g_activeTerrainRegionOwner;

// GLOBAL: LEGORACERS2 0x0067b858
Terrain* g_activeTerrainObject;

// GLOBAL: LEGORACERS2 0x0067b85c
void* g_activeTerrainCellCode;

// GLOBAL: LEGORACERS2 0x0067ba70
LegoFloat g_activeTerrainMinimumSample;

// STUB: LEGORACERS2 0x004d3e60
LegoBool32 __cdecl IsTerrainRegionRayCell(LegoS32 p_column, LegoS32 p_row)
{
	TerrainRegionOwnerView* owner = (TerrainRegionOwnerView*) g_activeTerrainRegionOwner;
	TerrainRegionTileView* tile;
	LegoS32 index;
	TerrainRegionCellView* cell;
	LegoFloat candidateHeight;

	index = (LegoS32) (((LegoU32) p_row << (owner->m_gridShift & 0x1f)) + (LegoU32) p_column);
	LegoS32 shift = *(LegoS32*) ((LegoU8*) owner + 0xf4);
	tile = GetRegionTile(
		g_activeTerrainObject,
		(LegoS32) ((LegoU32) p_column << (shift & 0x1f)),
		(LegoS32) ((LegoU32) p_row << (shift & 0x1f))
	);
	if ((tile->m_typeFlags & 0xfe) == 0x0e) {
		cell = &owner->m_levelRecords[0][index];
		candidateHeight = cell->m_centerY + cell->m_halfExtentY;
		if (candidateHeight < g_activeTerrainMinimumSample) {
			g_activeTerrainMinimumSample = candidateHeight;
			return TRUE;
		}
		return TRUE;
	}

	if (FindTerrainRegionCellData(owner, NULL, p_column, p_row) != g_activeTerrainCellCode) {
		g_activeTerrainMinimumSample = 2147483648.0f;
		return FALSE;
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004d3f00
void __fastcall CollectTerrainRegionCandidates(void* p_owner, void*, const LegoFloat* p_position)
{
	TerrainRegionOwnerView* owner = (TerrainRegionOwnerView*) p_owner;
	LegoFloat scaledX = owner->m_inverseCellWidth * p_position[0];
	LegoFloat scaledZ = owner->m_inverseCellDepth * p_position[2];
	LegoU8* candidate = (LegoU8*) owner->m_candidateTiles;

	g_activeTerrainRegionOwner = owner;
	g_activeTerrainObject = owner->m_terrain;
	owner->m_candidateOutputCount = 0;
	for (LegoS32 tileIndex = 0; tileIndex < owner->m_candidateTileCount; ++tileIndex, candidate += 0x10) {
		LegoS32 column = *(LegoS32*) candidate;
		LegoS32 row = *(LegoS32*) (candidate + 4);
		LegoS32 cellIndex = (LegoS32) (((LegoU32) row << (owner->m_gridShift & 0x1f)) + (LegoU32) column);
		TerrainRegionCellView* cell = &owner->m_levelRecords[0][cellIndex];
		LegoFloat cellTop = cell->m_centerY + cell->m_halfExtentY;

		if ((((TerrainVisibilityContext*) owner)->GetCellState(column, row) & 4) != 0) {
			continue;
		}
		g_activeTerrainCellCode = FindTerrainRegionCellData(owner, NULL, column, row);
		for (LegoU32 corner = 0; corner < 4; ++corner) {
			g_activeTerrainMinimumSample = -2147483648.0f;
			TraverseTerrainGridRay(
				scaledX,
				scaledZ,
				(float) ((corner & 1) + column) - scaledX,
				(float) (((corner >> 1) & 1) + row) - scaledZ,
				owner->m_gridWidth,
				owner->m_gridDepth,
				IsTerrainRegionRayCell
			);
			if (cellTop < g_activeTerrainMinimumSample) {
				LegoU32 outputOffset = ((LegoU32) row * (LegoU32) owner->m_gridWidth + (LegoU32) column) *
									   (LegoU32) owner->m_levelCount * 8;
				LegoU8* outputCell = (LegoU8*) owner->m_levelEntries + outputOffset;
				((void**) owner->m_candidateOutput)[owner->m_candidateOutputCount++] = outputCell;
				break;
			}
		}
	}
}

// STUB: LEGORACERS2 0x004f88f0
LegoU32 __cdecl TraverseTerrainGridRay(
	LegoFloat p_startX,
	LegoFloat p_startY,
	LegoFloat p_deltaX,
	LegoFloat p_deltaY,
	LegoS32 p_width,
	LegoS32 p_height,
	LegoBool32(__cdecl* p_testCell)(LegoS32, LegoS32)
)
{
	// The retail walker first clips the ray to the map rectangle, then visits
	// cell centers in increasing parametric distance. Keep the helper calls
	// explicit because they also preserve the original boundary behavior.
	LegoFloat maxX = (LegoFloat) p_width;
	LegoFloat maxY = (LegoFloat) p_height;
	LegoFloat tEnter = 0.0f;
	LegoFloat tExit = 1.0f;
	if (!ClipTerrainGridSegment(&p_startX, &p_startY, &p_deltaX, &p_deltaY, maxX, maxY)) {
		return 1;
	}
	LegoS32 column = RoundFloatUsingCurrentFpuMode(p_startX - 0.5f);
	LegoS32 row = RoundFloatUsingCurrentFpuMode(p_startY - 0.5f);
	if (p_testCell(column, row) == FALSE) {
		return 1;
	}
	for (;;) {
		LegoFloat tx = 1000.0f;
		LegoFloat ty = 1000.0f;
		LegoS32 stepX = 0;
		LegoS32 stepY = 0;
		if (p_deltaX > 0.001f) {
			stepX = 1;
			tx = ((LegoFloat) column + 1.0f - p_startX) / p_deltaX;
		}
		else if (p_deltaX < -0.001f) {
			stepX = -1;
			tx = ((LegoFloat) column - p_startX) / p_deltaX;
		}
		if (p_deltaY > 0.001f) {
			stepY = 1;
			ty = ((LegoFloat) row + 1.0f - p_startY) / p_deltaY;
		}
		else if (p_deltaY < -0.001f) {
			stepY = -1;
			ty = ((LegoFloat) row - p_startY) / p_deltaY;
		}
		if (tx <= ty) {
			if (tx > 1000.0f || tx > 1.0f) {
				return 1;
			}
			column += stepX;
		}
		else {
			if (ty > 1000.0f || ty > 1.0f) {
				return 1;
			}
			row += stepY;
		}
		if (p_testCell(column, row) == FALSE) {
			return 1;
		}
	}
}

// STUB: LEGORACERS2 0x004f8ab0
LegoBool32 __cdecl ClipTerrainGridSegment(
	LegoFloat* p_startX,
	LegoFloat* p_startY,
	LegoFloat* p_deltaX,
	LegoFloat* p_deltaY,
	LegoFloat p_width,
	LegoFloat p_height
)
{
	if ((*p_deltaX <= 0.0001f && *p_deltaX >= -0.0001f) && (*p_deltaY <= 0.0001f && *p_deltaY >= -0.0001f)) {
		return TRUE;
	}
	LegoFloat xEnter;
	LegoFloat xExit;
	LegoFloat yEnter;
	LegoFloat yExit;
	LegoFloat oldX = *p_startX;
	LegoFloat oldY = *p_startY;
	LegoFloat endX = oldX + *p_deltaX;
	LegoFloat endY = oldY + *p_deltaY;
	if (!ClipTerrainGridBoundary(oldX, endX, p_width, &xEnter, &xExit) ||
		!ClipTerrainGridBoundary(oldY, endY, p_height, &yEnter, &yExit)) {
		return FALSE;
	}
	LegoFloat enter = (xEnter < yEnter) ? yEnter : xEnter;
	LegoFloat exit = (xExit < yExit) ? xExit : yExit;
	if (!(exit > enter)) {
		return FALSE;
	}
	*p_startX = oldX + *p_deltaX * enter;
	*p_startY = oldY + *p_deltaY * enter;
	*p_deltaX *= exit - enter;
	*p_deltaY *= exit - enter;
	return TRUE;
}

// STUB: LEGORACERS2 0x004f8c40
LegoBool32 __cdecl ClipTerrainGridBoundary(
	LegoFloat p_start,
	LegoFloat p_end,
	LegoFloat p_boundary,
	LegoFloat* p_tEnter,
	LegoFloat* p_tExit
)
{
	LegoFloat minimum = 0.5f;
	LegoFloat maximum = p_boundary - 0.5f;
	LegoFloat delta = p_end - p_start;
	*p_tEnter = 0.0f;
	*p_tExit = 1.0f;
	if (p_start < minimum) {
		if (p_end < minimum) {
			return FALSE;
		}
		*p_tEnter = (minimum - p_start) / delta;
	}
	else if (p_start > maximum) {
		if (p_end > maximum) {
			return FALSE;
		}
		*p_tEnter = (maximum - p_start) / delta;
	}
	if (p_end < minimum) {
		*p_tExit = (minimum - p_start) / delta;
	}
	else if (p_end > maximum) {
		*p_tExit = (maximum - p_start) / delta;
	}
	if (*p_tEnter > *p_tExit) {
		return FALSE;
	}
	return TRUE;
}
