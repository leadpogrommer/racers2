#ifndef LR2_GAMEOBJECTTERRAINVISIBILITY_H
#define LR2_GAMEOBJECTTERRAINVISIBILITY_H

#include "types.h"

class Vector3;

// These owner-relative callbacks are called by the region visibility path.
// Their receiver remains an opaque owner view pending full class ownership.
LegoBool32 __fastcall UpdateTerrainRegionVisibility(void* p_owner, void*, void* p_view);
LegoU32 __fastcall RebuildTerrainRegionVisibility(void* p_owner, void*, void* p_view);
void __fastcall CollectTerrainRegionCandidates(void* p_owner, void*, const LegoFloat* p_position);
LegoU32 __fastcall ClassifyTerrainRegionCell(void* p_owner, void*, void* p_cell);
void __fastcall UpdateTerrainRegionCellNeighbors(
	void* p_owner,
	void*,
	LegoS32 p_level,
	LegoS32 p_column,
	LegoS32 p_row
);
LegoU8* __fastcall FindTerrainRegionCellData(void* p_owner, void*, LegoS32 p_column, LegoS32 p_row);
void __fastcall BuildTerrainCellCorners(void* p_cell, void*, Vector3* p_points);

#endif // LR2_GAMEOBJECTTERRAINVISIBILITY_H
