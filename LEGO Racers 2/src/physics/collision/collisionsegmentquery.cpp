#include "game/objects/gamemanager.h"
#include "physics/collision/collisionqueries.h"

struct CollisionGeometryView;
LegoBool32 __fastcall IntersectCollisionGeometrySegment(
	CollisionGeometryView*,
	void*,
	const Vector3*,
	const Vector3*,
	CollisionQueryResult*
);

// STUB: LEGORACERS2 0x004d8190
LegoBool32 CollisionModelView::IntersectSegment(
	const Vector3* p_start,
	const Vector3* p_delta,
	CollisionQueryResult* p_result
)
{
	return IntersectCollisionGeometrySegment((CollisionGeometryView*) &m_unk0x04, NULL, p_start, p_delta, p_result);
}

// STUB: LEGORACERS2 0x004d81b0
LegoBool32 CollisionProxy::IntersectSegment(
	const Vector3* p_start,
	const Vector3* p_delta,
	CollisionQueryResult* p_result
)
{
	Matrix3 inverse;
	inverse.FUN_004a5040(&m_rotation);
	Vector3 offset = m_position;
	offset.FUN_004a4e00(&inverse);
	offset.m_x *= -1.0f;
	offset.m_y *= -1.0f;
	offset.m_z *= -1.0f;
	Vector3 start = *p_start;
	Vector3 delta = *p_delta;
	start.FUN_004a4e00(&inverse);
	start.m_x += offset.m_x;
	start.m_y += offset.m_y;
	start.m_z += offset.m_z;
	delta.FUN_004a4e00(&inverse);
	if (!m_model->IntersectSegment(&start, &delta, p_result)) {
		return FALSE;
	}
	p_result->m_point.FUN_004a4e00(&m_rotation);
	p_result->m_point.m_x += m_position.m_x;
	p_result->m_point.m_y += m_position.m_y;
	p_result->m_point.m_z += m_position.m_z;
	p_result->m_normal.FUN_004a4e00(&m_rotation);
	p_result->m_first = (GameLifecycleComponent*) m_tag;
	p_result->m_second = (GameLifecycleComponent*) m_tag;
	return TRUE;
}

// STUB: LEGORACERS2 0x004f53e0
LegoU32 __fastcall QueryCollisionSegment(
	CollisionSpatialIndex* p_index,
	void*,
	const Vector3* p_start,
	const Vector3* p_end,
	CollisionQueryResult* p_result
)
{
	CollisionQueryBounds bounds;
	InitializeCollisionQueryBoundsFromEndpoints(&bounds, NULL, p_start, p_end);
	Vector3 delta = *p_end;
	delta.m_x -= p_start->m_x;
	delta.m_y -= p_start->m_y;
	delta.m_z -= p_start->m_z;
	p_result->m_penetration = 1.0f;
	LegoU32 dynamicHit = p_index->m_dynamicIndex->IntersectSegment(&bounds, p_start, &delta, p_result);
	LegoU32 staticHit = p_index->m_staticGrid->IntersectSegment(&bounds, p_start, &delta, p_result);
	((SpatialIndexLink*) &bounds)->Detach();
	return staticHit | dynamicHit;
}

LegoU32 __cdecl TraverseTerrainGridRay(
	LegoFloat,
	LegoFloat,
	LegoFloat,
	LegoFloat,
	LegoS32,
	LegoS32,
	LegoBool32(__cdecl*)(LegoS32, LegoS32)
);

// STUB: LEGORACERS2 0x004f8da0
static void __cdecl TraverseCollisionGridRay(
	LegoFloat p_x,
	LegoFloat p_z,
	LegoFloat p_deltaX,
	LegoFloat p_deltaZ,
	LegoFloat p_originX,
	LegoFloat p_originZ,
	LegoFloat p_width,
	LegoFloat p_depth,
	LegoS32 p_columns,
	LegoS32 p_rows,
	LegoBool32(__cdecl* p_visit)(LegoS32, LegoS32)
)
{
	TraverseTerrainGridRay(
		(p_x - p_originX) * ((LegoFloat) p_columns / p_width),
		(p_z - p_originZ) * ((LegoFloat) p_rows / p_depth),
		((LegoFloat) p_columns / p_width) * p_deltaX,
		((LegoFloat) p_rows / p_depth) * p_deltaZ,
		p_columns,
		p_rows,
		p_visit
	);
}

// FUNCTION: LEGORACERS2 0x00507e60
LegoBool32 DynamicSpatialIndex::IntersectSegment(
	const CollisionQueryBounds* p_bounds,
	const Vector3* p_start,
	const Vector3* p_delta,
	CollisionQueryResult* p_result
)
{
	LegoU32 candidates[256];
	candidates[0] = 256;
	LegoS32 count = CollectCandidates(p_bounds, candidates);
	LegoBool32 hit = FALSE;
	for (LegoS32 i = 0; i < count; ++i) {
		GameLifecycleComponent* component = (GameLifecycleComponent*) candidates[i];
		if (component->m_collisionProxy->IntersectSegment(p_start, p_delta, p_result)) {
			hit = TRUE;
		}
	}
	return hit;
}

// The visited array and following delta pointer are adjacent retail storage;
// the initialized limit pointer addresses this exact end, not an unrelated BSS symbol.
struct CollisionSegmentScratch {
	CollisionProxy* m_visited[128];
	const Vector3* m_delta;
};
DECOMP_STATIC_ASSERT(offsetof(CollisionSegmentScratch, m_delta) == 0x200);
DECOMP_SIZE_ASSERT(CollisionSegmentScratch, 0x204);
// GLOBAL: LEGORACERS2 0x006fd5bc
static CollisionSegmentScratch g_collisionSegmentScratch;
// GLOBAL: LEGORACERS2 0x0054c100
static CollisionProxy** g_segmentProxyEnd = g_collisionSegmentScratch.m_visited;
// GLOBAL: LEGORACERS2 0x0054c104
static CollisionProxy** g_segmentProxyLimit = g_collisionSegmentScratch.m_visited + 128;
// GLOBAL: LEGORACERS2 0x006fd7c0
static LegoBool32 g_collisionSegmentHit;
// GLOBAL: LEGORACERS2 0x006fdfc4
static const Vector3* g_collisionSegmentStart;
// GLOBAL: LEGORACERS2 0x006fdfcc
static CollisionQueryResult* g_collisionSegmentResult;
// GLOBAL: LEGORACERS2 0x006fdfd0
static StaticSpatialGrid* g_collisionSegmentGrid;

static LegoBool32 __cdecl VisitCollisionSegmentCell(LegoS32, LegoS32);

// FUNCTION: LEGORACERS2 0x005088f0
LegoBool32 StaticSpatialGrid::IntersectSegment(
	const CollisionQueryBounds* p_bounds,
	const Vector3* p_start,
	const Vector3* p_delta,
	CollisionQueryResult* p_result
)
{
	g_collisionSegmentStart = p_start;
	g_collisionSegmentScratch.m_delta = p_delta;
	g_collisionSegmentResult = p_result;
	g_collisionSegmentHit = FALSE;
	g_segmentProxyEnd = g_collisionSegmentScratch.m_visited;
	g_segmentProxyLimit = g_collisionSegmentScratch.m_visited + 128;
	g_collisionSegmentGrid = this;
	TraverseCollisionGridRay(
		p_start->m_x,
		p_start->m_z,
		p_delta->m_x,
		p_delta->m_z,
		m_origin.m_x,
		m_origin.m_z,
		m_diameter,
		m_diameter,
		m_size,
		m_size,
		VisitCollisionSegmentCell
	);
	return g_collisionSegmentHit;
}

// FUNCTION: LEGORACERS2 0x00508970
static LegoBool32 __cdecl VisitCollisionSegmentCell(LegoS32 p_x, LegoS32 p_z)
{
	for (SpatialSlotNode* node = g_collisionSegmentGrid->GetCell(p_x, p_z)->m_head; node != NULL; node = node->m_next) {
		CollisionProxy* proxy = ((SpatialIndexLink*) node->m_value)->m_proxy;
		CollisionProxy** previous;
		for (previous = g_collisionSegmentScratch.m_visited; previous != g_segmentProxyEnd; ++previous) {
			if (*previous == proxy) {
				break;
			}
		}
		if (previous != g_segmentProxyEnd) {
			continue;
		}
		if (g_segmentProxyEnd == g_segmentProxyLimit) {
			return FALSE;
		}
		*g_segmentProxyEnd++ = proxy;
		if (proxy->IntersectSegment(
				g_collisionSegmentStart,
				g_collisionSegmentScratch.m_delta,
				g_collisionSegmentResult
			)) {
			g_collisionSegmentHit = TRUE;
		}
	}
	return TRUE;
}
