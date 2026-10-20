#ifndef LR2_COLLISIONQUERIES_H
#define LR2_COLLISIONQUERIES_H

#include "physics/collision/collisioncontacts.h"
#include "physics/collision/collisionspatialindex.h"

struct CollisionQueryBounds {
	undefined m_unk0x00[0x1c];
	LegoFloat m_minX; // 0x1c
	LegoFloat m_minZ; // 0x20
	LegoFloat m_maxX; // 0x24
	LegoFloat m_maxZ; // 0x28
	undefined m_unk0x2c[0x08];
};
DECOMP_SIZE_ASSERT(CollisionQueryBounds, 0x34);

// Ray queries use the same 0x4c record as contact generation. The +0x24
// field is a parametric distance here; sphere rays also update flags at +0x4a.
struct CollisionQueryResult : CollisionContact {};
DECOMP_SIZE_ASSERT(CollisionQueryResult, 0x4c);

struct CollisionPairQueryState {
	CollisionContact m_records[256];
	LegoS32 m_currentLinkCount;
	union {
		CollisionProxy* m_firstProxy;
		LegoU32 m_unk0x4c04;
	}; // 0x4c04
	union {
		CollisionProxy* m_secondProxy;
		LegoU32 m_unk0x4c08;
	}; // 0x4c08
	CollisionModelTransform m_firstTransform;
	CollisionModelTransform m_secondTransform;
	LegoU32 m_unk0x4c6c;
};
DECOMP_SIZE_ASSERT(CollisionPairQueryState, 0x4c70);

extern CollisionPairQueryState* g_collisionServiceState;

CollisionQueryBounds* __fastcall InitializeCollisionQueryBoundsFromEndpoints(
	CollisionQueryBounds* p_bounds,
	void*,
	const Vector3* p_first,
	const Vector3* p_second
);
CollisionQueryBounds* __fastcall InitializeCollisionQueryBoundsFromRadius(
	CollisionQueryBounds* p_bounds,
	void*,
	const Vector3* p_center,
	LegoFloat p_radius
);

void __fastcall PrepareCollisionSpatialQueries(CollisionSpatialIndex* p_index, void*);
LegoU32 __fastcall QueryCollisionSphere(
	CollisionSpatialIndex* p_index,
	void*,
	const Vector3* p_center,
	LegoFloat p_radius,
	LegoU32* p_tags
);
LegoU32 __fastcall QueryCollisionSegment(
	CollisionSpatialIndex* p_index,
	void*,
	const Vector3* p_start,
	const Vector3* p_end,
	CollisionQueryResult* p_result
);
LegoBool32 __stdcall IsCollisionSweepPositionClear(CollisionProxy* p_proxy);

void __fastcall CollectGridCollisionCandidates(StaticSpatialGrid* p_grid, void*, SpatialIndexLink* p_link);
LegoBool32 __fastcall TryRecordCollisionPair(
	CollisionPairQueryState* p_state,
	void*,
	CollisionProxy* p_first,
	CollisionProxy* p_second
);

#endif // LR2_COLLISIONQUERIES_H
