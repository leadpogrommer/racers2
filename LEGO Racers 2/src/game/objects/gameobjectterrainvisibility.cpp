#include "game/objects/gameobjectterrainvisibility.h"

#include "math/matrix3.h"
#include "math/vector3.h"
#include "render/core/renderer.h"
#include "render/core/rendererconstructiondispatch.h"
#include "render/terrain/rendererterrain.h"

#include <stddef.h>

namespace
{

struct RegionRecordView {
	LegoFloat m_centerX;
	LegoFloat m_centerY;
	LegoFloat m_centerZ;
	LegoFloat m_halfHeight;
	LegoFloat m_halfExtentX;
	LegoFloat m_halfExtentY;
	LegoFloat m_halfExtentZ;
	LegoFloat m_unk0x1c;
};

struct RegionTerrainView {
	Terrain* m_terrain;        // +0x00
	LegoU32 m_geometryReady;   // +0x04
	LegoU32 m_transformMode;   // +0x08
	LegoU32 m_shadowHit;       // +0x0c
	Vector3 m_transformOrigin; // +0x10
	Vector3 m_gridOrigin;      // +0x1c
	Vector3 m_gridCenter;      // +0x28
	LegoU8 m_opaque0x34[0x10];
	Matrix3 m_transform;                  // +0x44
	RegionRecordView* m_levelRecords[16]; // +0x68
	LegoU32 m_gridShift;                  // +0xa8
	LegoU8* m_levelEntries;               // +0xac
	void* m_currentMap;                   // +0xb0
	void* m_nextMap;                      // +0xb4
	LegoU8 m_opaque0xb8[0x08];
	LegoU32 m_levelCount; // +0xc0
	LegoU8 m_opaque0xc4[0x04];
	LegoFloat m_verticalOffset;  // +0xc8
	LegoFloat m_verticalPadding; // +0xcc
	LegoS32 m_gridWidth;         // +0xd0
	LegoS32 m_gridDepth;         // +0xd4
	LegoU8 m_opaque0xd8[0x08];
	LegoFloat m_inverseCellWidth; // +0xe0
	LegoFloat m_inverseCellDepth; // +0xe4
	LegoU8 m_opaque0xe8[0x10];
	void* m_candidateTiles;                          // +0xf8
	LegoS32 m_candidateTileCount;                    // +0xfc
	void* m_candidateOutput;                         // +0x100
	LegoS32 m_candidateOutputCount;                  // +0x104
	LegoU8 m_cullingPlanes[0x90];                    // +0x108
	LegoU32 m_cullingPlanePointer;                   // +0x198
	LegoU32 m_cullingPlaneCount;                     // +0x19c
	RendererTerrainGeometryPacket* m_geometryPacket; // +0x1a0
};

DECOMP_STATIC_ASSERT(offsetof(RegionTerrainView, m_levelRecords) == 0x68);
DECOMP_STATIC_ASSERT(offsetof(RegionTerrainView, m_gridShift) == 0xa8);
DECOMP_STATIC_ASSERT(offsetof(RegionTerrainView, m_levelEntries) == 0xac);
DECOMP_STATIC_ASSERT(offsetof(RegionTerrainView, m_currentMap) == 0xb0);
DECOMP_STATIC_ASSERT(offsetof(RegionTerrainView, m_levelCount) == 0xc0);
DECOMP_STATIC_ASSERT(offsetof(RegionTerrainView, m_inverseCellWidth) == 0xe0);
DECOMP_STATIC_ASSERT(offsetof(RegionTerrainView, m_candidateTiles) == 0xf8);
DECOMP_STATIC_ASSERT(offsetof(RegionTerrainView, m_candidateOutputCount) == 0x104);
DECOMP_STATIC_ASSERT(offsetof(RegionTerrainView, m_cullingPlanePointer) == 0x198);
DECOMP_STATIC_ASSERT(offsetof(RegionTerrainView, m_geometryPacket) == 0x1a0);

struct RegionCameraView {
	LegoU8 m_opaque0x000[0x188];
	Vector3 m_position;
	LegoFloat m_orientation[4];
	LegoU8 m_opaque0x1a4[0x34];
	Vector3 m_frustumNormals[5];
	LegoU8 m_opaque0x214[0x0c];
	Vector3 m_frustumPoints[5];
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

static LegoS32 GetRegionCellIndex(LegoS32 p_column, LegoS32 p_row, LegoU32 p_shift)
{
	return (p_row << (p_shift & 0x1f)) + p_column;
}

} // namespace

// STUB: LEGORACERS2 0x004d3a60
LegoBool32 __fastcall UpdateTerrainRegionVisibility(void* p_owner, void*, void* p_view)
{
	RegionTerrainView* owner = (RegionTerrainView*) p_owner;
	RegionCameraView* view = (RegionCameraView*) p_view;
	Vector3 localPosition = view->m_position;
	LegoS32 column;
	LegoS32 row;
	LegoS32 cellIndex;
	RegionRecordView* record;

	owner->m_candidateOutputCount = 0;
	owner->m_currentMap = owner->m_nextMap;
	owner->m_shadowHit = 0;
	if (owner->m_geometryReady == 0) {
		RebuildTerrainRegionVisibility(p_owner, NULL, p_view);
		return FALSE;
	}

	if (owner->m_transformMode == 1) {
		localPosition.m_x -= owner->m_transformOrigin.m_x;
		localPosition.m_y -= owner->m_transformOrigin.m_y;
		localPosition.m_z -= owner->m_transformOrigin.m_z;
		localPosition.FUN_004a4e00(&owner->m_transform);
	}
	localPosition.m_x -= owner->m_gridOrigin.m_x;
	localPosition.m_y -= owner->m_gridOrigin.m_y;
	localPosition.m_z -= owner->m_gridOrigin.m_z;
	column = RoundFloatUsingCurrentFpuMode(localPosition.m_x * owner->m_inverseCellWidth - 0.5f);
	row = RoundFloatUsingCurrentFpuMode(localPosition.m_z * owner->m_inverseCellDepth - 0.5f);

	if (column < 0 || row < 0 || column >= owner->m_gridWidth || row >= owner->m_gridDepth) {
		RebuildTerrainRegionVisibility(p_owner, NULL, p_view);
		return FALSE;
	}

	localPosition.m_x += owner->m_gridOrigin.m_x;
	localPosition.m_z += owner->m_gridOrigin.m_z;
	cellIndex = GetRegionCellIndex(column, row, owner->m_gridShift);
	record = owner->m_levelRecords[0] + cellIndex;
	if (owner->m_verticalOffset + record->m_halfHeight + record->m_halfExtentY < localPosition.m_y) {
		RebuildTerrainRegionVisibility(p_owner, NULL, p_view);
		return FALSE;
	}

	if (record->m_halfHeight - record->m_halfExtentY <= localPosition.m_y) {
		void* firstState = *(void**) ((LegoU8*) owner->m_geometryPacket + 0x0c);
		void* secondState = *(void**) ((LegoU8*) owner->m_geometryPacket + 0x10);
		void** firstVtable = *(void***) firstState;
		void** secondVtable = *(void***) secondState;
		typedef void(__fastcall * State4)(void*, void*, LegoU32, LegoU32, LegoU32, LegoU32);
		typedef void(__fastcall * State3)(void*, void*, LegoU32, LegoU32, LegoU32);
		typedef void(__fastcall * State2)(void*, void*, LegoU32, LegoU32);
		typedef void(__fastcall * State1)(void*, void*, LegoU32);
		State4 beginFirst = (State4) firstVtable[1];
		State3 beginSecond = (State3) secondVtable[1];
		State2 endSecond = (State2) secondVtable[2];
		State1 endFirst = (State1) firstVtable[2];
		RendererMainDispatch* rendererDispatch = (RendererMainDispatch*) g_displayRenderer->m_vtable;
		Vector3 outputPoint;

		beginFirst(firstState, NULL, 0, 0, 1, 0);
		beginSecond(secondState, NULL, 0, 0, 1);
		rendererDispatch
			->m_buildTerrainGeometry(owner->m_terrain, &outputPoint, &view->m_position, 1, owner->m_geometryPacket, 1);

		if (owner->m_geometryPacket->m_triangleCount < 2) {
			owner->m_shadowHit = 1;
		}
		else {
			RendererIndexBuffer* indices = owner->m_geometryPacket->m_indexBuffer;
			LegoU16* triangle = (LegoU16*) indices->m_data;
			for (LegoS32 i = 0; i < (LegoU16) owner->m_geometryPacket->m_triangleCount; ++i, triangle += 3) {
				Vector3* first = owner->m_geometryPacket->m_vertexBuffer->GetPosition(triangle[0]);
				Vector3* second = owner->m_geometryPacket->m_vertexBuffer->GetPosition(triangle[1]);
				Vector3* third = owner->m_geometryPacket->m_vertexBuffer->GetPosition(triangle[2]);
				LegoFloat edge1X = second->m_x - first->m_x;
				LegoFloat edge1Y = second->m_y - first->m_y;
				LegoFloat edge1Z = second->m_z - first->m_z;
				LegoFloat edge2X = third->m_x - first->m_x;
				LegoFloat edge2Y = third->m_y - first->m_y;
				LegoFloat edge2Z = third->m_z - first->m_z;
				LegoFloat normalX = edge2Z * edge1Y - edge2Y * edge1Z;
				LegoFloat normalY = edge2X * edge1Z - edge2Z * edge1X;
				LegoFloat normalZ = edge2Y * edge1X - edge2X * edge1Y;
				LegoFloat deltaX = localPosition.m_x - first->m_x;
				LegoFloat deltaY = localPosition.m_y - first->m_y;
				LegoFloat deltaZ = localPosition.m_z - first->m_z;
				if (!(normalX * deltaX + normalY * deltaY + normalZ * deltaZ > 0.0f)) {
					owner->m_shadowHit = 1;
					break;
				}
			}
		}
		endSecond(secondState, NULL, 0, 1);
		endFirst(firstState, NULL, 0);
	}
	else {
		owner->m_shadowHit = 1;
	}

	if (owner->m_shadowHit == 0) {
		LegoU8* gridMap = owner->m_levelEntries;
		owner->m_currentMap = gridMap + (owner->m_gridWidth * row + column) * owner->m_levelCount * 8;
	}
	RebuildTerrainRegionVisibility(p_owner, NULL, p_view);
	if (owner->m_shadowHit != 1) {
		return TRUE;
	}
	CollectTerrainRegionCandidates(p_owner, NULL, &localPosition.m_x);
	return FALSE;
}

// STUB: LEGORACERS2 0x004d3e10
LegoU8* __fastcall FindTerrainRegionCellData(void* p_owner, void*, LegoS32 p_column, LegoS32 p_row)
{
	RegionTerrainView* owner = (RegionTerrainView*) p_owner;
	LegoU8* candidate = (LegoU8*) owner->m_candidateTiles;
	for (LegoS32 i = 0; i < owner->m_candidateTileCount; ++i, candidate += 0x10) {
		if (*(LegoS32*) candidate == p_column && *(LegoS32*) (candidate + 4) == p_row) {
			return candidate + 0x0c;
		}
	}
	return NULL;
}

// STUB: LEGORACERS2 0x004d46a0
LegoU32 __fastcall RebuildTerrainRegionVisibility(void* p_owner, void*, void* p_view)
{
	RegionTerrainView* owner = (RegionTerrainView*) p_owner;
	RegionCameraView* view = (RegionCameraView*) p_view;
	Matrix3 cameraOrientation;
	Matrix3 combinedOrientation;
	Vector3 cameraOffset = view->m_position;
	LegoS32 lastLevel = owner->m_levelCount - 1;
	LegoS32 neighborLevel = owner->m_levelCount - 2;
	LegoS32 cellWidth = owner->m_gridWidth >> (lastLevel & 0x1f);
	LegoS32 cellDepth = owner->m_gridDepth >> (lastLevel & 0x1f);
	RegionRecordView* cell = owner->m_levelRecords[lastLevel];

	cameraOrientation.FUN_004a4d30(view->m_orientation);
	combinedOrientation.FUN_004a4ea0(&cameraOrientation, &owner->m_transform);
	cameraOffset.m_x -= owner->m_transformOrigin.m_x;
	cameraOffset.m_y -= owner->m_transformOrigin.m_y;
	cameraOffset.m_z -= owner->m_transformOrigin.m_z;
	cameraOffset.FUN_004a4e00(&owner->m_transform);

	owner->m_cullingPlanePointer = (LegoU32) ((LegoU8*) owner + 0x144);
	owner->m_cullingPlaneCount = 5;
	for (LegoS32 i = 0; i < 5; ++i) {
		Vector3 normal = view->m_frustumNormals[i];
		Vector3 point = view->m_frustumPoints[i];
		normal.FUN_004a4e00(&combinedOrientation);
		point.FUN_004a4e00(&combinedOrientation);
		point.m_x += view->m_position.m_x - owner->m_transformOrigin.m_x;
		point.m_y += view->m_position.m_y - owner->m_transformOrigin.m_y;
		point.m_z += view->m_position.m_z - owner->m_transformOrigin.m_z;
		((Vector3*) ((LegoU8*) owner + 0x108))[i] = normal;
		((Vector3*) ((LegoU8*) owner + 0x144))[i] = point;
	}

	for (LegoS32 row = 0; row < cellDepth; ++row) {
		for (LegoS32 column = 0; column < cellWidth; ++column, ++cell) {
			LegoU32 state = ClassifyTerrainRegionCell(p_owner, NULL, cell);
			if (state == 8 && neighborLevel >= 0) {
				UpdateTerrainRegionCellNeighbors(p_owner, NULL, neighborLevel, column, row);
			}
			*(LegoU32*) ((LegoU8*) cell + 0x1c) = state;
		}
	}
	return 1;
}
