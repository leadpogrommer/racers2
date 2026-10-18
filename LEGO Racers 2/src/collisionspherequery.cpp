#include "collisionqueries.h"

// STUB: LEGORACERS2 0x004f53a0
LegoU32 __fastcall QueryCollisionSphere(
	CollisionSpatialIndex* p_index,
	void*,
	const Vector3* p_center,
	LegoFloat p_radius,
	LegoU32* p_tags
)
{
	CollisionQueryBounds bounds;
	InitializeCollisionQueryBoundsFromRadius(&bounds, NULL, p_center, p_radius);
	LegoU32 result = p_index->m_dynamicIndex->CollectCandidates(&bounds, p_tags);
	// Both temporary query constructors zero the SpatialIndexLink prefix;
	// the retail destructor still attempts its ordinary detach operation.
	((SpatialIndexLink*) &bounds)->Detach();
	return result;
}
