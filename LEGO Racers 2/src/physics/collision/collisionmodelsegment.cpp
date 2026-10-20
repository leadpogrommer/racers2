#include "memory/memoryheap.h"
#include "physics/collision/collisiongeometry.h"

#include <math.h>
#include <string.h>

struct CollisionTriangleTree {
	const LegoU16* m_words;
	LegoU32 m_unk0x04;
	const CollisionTriangleRecord* m_records;
	undefined m_unk0x0c[0x18 - 0x0c];
	Vector3 m_center;
	Vector3 m_halfExtent;
	Vector3 m_childOffsets[8];
	LegoS32 m_childCount;
	LegoU32 m_splitAxes;
};
DECOMP_STATIC_ASSERT(offsetof(CollisionTriangleTree, m_childOffsets) == 0x30);
DECOMP_SIZE_ASSERT(CollisionTriangleTree, 0x98);

struct CollisionTriangleCandidates : CollisionTriangleIndexList {
	CollisionTriangleCandidates();
	LegoBool32 Insert(LegoU32, CollisionTriangleIndexNode**);
	void AppendRange(const LegoU16*, LegoS32, CollisionTriangleIndexNode**);
	void AddRange(const LegoU16*, LegoU32);
	LegoS32 m_count;
	LegoS32 m_capacity;
	LegoBool32 m_rejectDuplicates;
};
DECOMP_SIZE_ASSERT(CollisionTriangleCandidates, 0x18);

// GLOBAL: LEGORACERS2 0x006fdfd8
static CollisionTriangleCandidates g_collisionTriangleCandidates;
// GLOBAL: LEGORACERS2 0x006fdff4
static LegoU8 g_collisionTriangleCandidatesReleased;

struct CollisionTreeQueueEntry {
	LegoU16 m_wordIndex;
	LegoU16 m_scale;
	Vector3 m_center;
};
DECOMP_SIZE_ASSERT(CollisionTreeQueueEntry, 0x10);

struct CollisionTreeQueue {
	CollisionTreeQueueEntry m_entries[256];
	CollisionTreeQueueEntry* m_read;
	CollisionTreeQueueEntry* m_write;
	LegoU32 m_readCount;
	LegoU32 m_writeCount;
	LegoU32 m_unk0x1010;
	void AdvanceWrite();
};
DECOMP_SIZE_ASSERT(CollisionTreeQueue, 0x1014);
// GLOBAL: LEGORACERS2 0x006fe000
static CollisionTreeQueue g_collisionTreeQueue;

LegoBool32 __fastcall IntersectCollisionGeometrySegment(
	CollisionGeometryView* p_geometry,
	void*,
	const Vector3* p_start,
	const Vector3* p_delta,
	CollisionQueryResult* p_result
);
LegoBool32 __fastcall IntersectTerrainCollisionSegment(
	void*,
	void*,
	const Vector3*,
	const Vector3*,
	CollisionQueryResult*
);
static LegoBool32 __fastcall IntersectCollisionTreeSegment(
	CollisionTriangleTree*,
	void*,
	const Vector3*,
	const Vector3*,
	CollisionQueryResult*
);

// STUB: LEGORACERS2 0x004fa210
LegoBool32 __fastcall IntersectCollisionGeometrySegment(
	CollisionGeometryView* p_geometry,
	void*,
	const Vector3* p_start,
	const Vector3* p_delta,
	CollisionQueryResult* p_result
)
{
	LegoBool32 hit = FALSE;
	for (LegoS32 i = 0; i < p_geometry->m_sphereCount; ++i) {
		const CollisionSphereRecord* sphere = (const CollisionSphereRecord*) p_geometry->m_sphereRecords + i;
		if (IntersectRayWithCollisionSphere(sphere, NULL, p_start, p_delta, p_result)) {
			hit = TRUE;
		}
	}
	if (p_geometry->m_unk0x00 != 0) {
		if (IntersectCollisionTreeSegment(
				(CollisionTriangleTree*) p_geometry->m_unk0x00,
				NULL,
				p_start,
				p_delta,
				p_result
			)) {
			return TRUE;
		}
	}
	else if (p_geometry->m_unk0x04 != NULL) {
		if (IntersectTerrainCollisionSegment(p_geometry->m_unk0x04, NULL, p_start, p_delta, p_result)) {
			return TRUE;
		}
	}
	return hit;
}

static void ReleaseTriangleCandidates();

// STUB: LEGORACERS2 0x00508da0
CollisionTriangleCandidates::CollisionTriangleCandidates()
{
	m_capacity = 500;
	m_nodes = (CollisionTriangleIndexNode*) GetMemoryHeap()->Allocate(m_capacity * 4, "New list of UINT16s.");
	m_end = m_nodes + 0xffff;
	m_rejectDuplicates = TRUE;
	m_count = 0;
	m_firstNodeIndex = 0xffff;
}

// STUB: LEGORACERS2 0x00508e10
static void ReleaseTriangleCandidates()
{
	if (!(g_collisionTriangleCandidatesReleased & 1)) {
		g_collisionTriangleCandidatesReleased |= 1;
		if (g_collisionTriangleCandidates.m_nodes != NULL) {
			GetMemoryHeap()->Free((void*) g_collisionTriangleCandidates.m_nodes);
		}
	}
}

// The original registers 508e10 through its CRT initializer at 508d90/508e00.
struct TriangleCandidatesExitRegistration {
	TriangleCandidatesExitRegistration();
};
#include <stdlib.h>
TriangleCandidatesExitRegistration::TriangleCandidatesExitRegistration()
{
	atexit(ReleaseTriangleCandidates);
}
static TriangleCandidatesExitRegistration g_triangleCandidatesExitRegistration;

// STUB: LEGORACERS2 0x0050a110
void CollisionTriangleCandidates::AddRange(const LegoU16* p_indices, LegoU32 p_count)
{
	LegoS32 count = (LegoS32) p_count;
	if (m_count + count > m_capacity) {
		count = m_capacity - m_count;
	}
	if (count <= 0) {
		return;
	}
	CollisionTriangleIndexNode* previous = (CollisionTriangleIndexNode*) this;
	if (m_count == 0) {
		AppendRange(p_indices, count, &previous);
	}
	else {
		for (; count > 0; --count, ++p_indices) {
			Insert(*p_indices, &previous);
		}
	}
}

// STUB: LEGORACERS2 0x0050a710
LegoBool32 CollisionTriangleCandidates::Insert(LegoU32 p_index, CollisionTriangleIndexNode** p_previous)
{
	CollisionTriangleIndexNode* nodes = (CollisionTriangleIndexNode*) m_nodes;
	if (m_count == 0) {
		if (m_capacity <= 0) {
			return FALSE;
		}
		nodes[0].m_nextIndex = m_firstNodeIndex;
		m_firstNodeIndex = (LegoU16) m_count;
	}
	else {
		CollisionTriangleIndexNode* next = nodes + (*p_previous)->m_nextIndex;
		LegoU32 value = p_index;
		while (next != m_end) {
			value = next->m_triangleIndex;
			if ((LegoS32) p_index <= (LegoS32) value) {
				break;
			}
			*p_previous = next;
			next = nodes + next->m_nextIndex;
		}
		if (value == p_index && m_rejectDuplicates) {
			return FALSE;
		}
		if (m_count >= m_capacity) {
			return FALSE;
		}
		nodes[m_count].m_nextIndex = (*p_previous)->m_nextIndex;
		(*p_previous)->m_nextIndex = (LegoU16) m_count;
	}
	nodes[m_count].m_triangleIndex = (LegoU16) p_index;
	++m_count;
	return TRUE;
}

// STUB: LEGORACERS2 0x0050a810
void CollisionTriangleCandidates::AppendRange(
	const LegoU16* p_indices,
	LegoS32 p_count,
	CollisionTriangleIndexNode** p_previous
)
{
	LegoU16 next = (*p_previous)->m_nextIndex;
	(*p_previous)->m_nextIndex = (LegoU16) m_count;
	CollisionTriangleIndexNode* nodes = (CollisionTriangleIndexNode*) m_nodes;
	*p_previous = nodes + m_count;
	for (; p_count > 1; --p_count, ++p_indices) {
		++m_count;
		(*p_previous)->m_triangleIndex = *p_indices;
		(*p_previous)->m_nextIndex = (LegoU16) m_count;
		++*p_previous;
	}
	(*p_previous)->m_triangleIndex = *p_indices;
	(*p_previous)->m_nextIndex = next;
	++m_count;
}

// FUNCTION: LEGORACERS2 0x0050a890
void CollisionTreeQueue::AdvanceWrite()
{
	++m_writeCount;
	m_write = &m_entries[m_writeCount & 0xff];
}

static void __fastcall CollectCollisionSegmentTriangles(
	CollisionTriangleTree*,
	void*,
	const Vector3*,
	const Vector3*,
	CollisionQueryResult*
);

// STUB: LEGORACERS2 0x0050ad20
static LegoBool32 __fastcall IntersectCollisionTreeSegment(
	CollisionTriangleTree* p_tree,
	void*,
	const Vector3* p_start,
	const Vector3* p_delta,
	CollisionQueryResult* p_result
)
{
	CollectCollisionSegmentTriangles(p_tree, NULL, p_start, p_delta, p_result);
	return TestIndexedCollisionTriangles(
		(const CollisionTriangleRecordBase*) &p_tree->m_records,
		NULL,
		p_start,
		p_delta,
		p_result,
		&g_collisionTriangleCandidates
	);
}

// STUB: LEGORACERS2 0x0050ad60
static LegoBool32 __cdecl IntersectSegmentBounds(
	const Vector3* p_start,
	const Vector3* p_delta,
	const LegoFloat* p_distance,
	const Vector3* p_center,
	const Vector3* p_halfExtent
)
{
	LegoFloat enter = 0.0f;
	LegoFloat exit = *p_distance;
	const LegoFloat* start = &p_start->m_x;
	const LegoFloat* delta = &p_delta->m_x;
	const LegoFloat* center = &p_center->m_x;
	const LegoFloat* halfExtent = &p_halfExtent->m_x;
	for (LegoS32 axis = 0; axis < 3; ++axis) {
		if ((LegoFloat) fabs(delta[axis]) >= 0.00001f) {
			LegoFloat first = ((center[axis] - halfExtent[axis]) - start[axis]) / delta[axis];
			LegoFloat second = ((center[axis] + halfExtent[axis]) - start[axis]) / delta[axis];
			if (second < first) {
				LegoFloat temporary = first;
				first = second;
				second = temporary;
			}
			if (exit < first || second < enter) {
				return FALSE;
			}
			if (enter < first) {
				enter = first;
			}
			if (second < exit) {
				exit = second;
			}
		}
		else if (start[axis] <= center[axis] - halfExtent[axis] || center[axis] + halfExtent[axis] <= start[axis]) {
			return FALSE;
		}
	}
	return !(exit < enter);
}

static void __fastcall GetCollisionChildHalfExtent(CollisionTriangleTree*, void*, LegoFloat, Vector3*);

// STUB: LEGORACERS2 0x0050b090
static void __fastcall CollectCollisionSegmentTriangles(
	CollisionTriangleTree* p_tree,
	void*,
	const Vector3* p_start,
	const Vector3* p_delta,
	CollisionQueryResult* p_result
)
{
	CollisionTriangleCandidates& candidates = g_collisionTriangleCandidates;
	candidates.m_count = 0;
	candidates.m_firstNodeIndex = 0xffff;
	LegoFloat distance = p_result->m_penetration;
	if (p_tree->m_words[0] == 0) {
		candidates.AddRange(p_tree->m_words + 2, p_tree->m_words[1]);
		return;
	}
	CollisionTreeQueue& queue = g_collisionTreeQueue;
	queue.m_readCount = queue.m_writeCount = queue.m_unk0x1010 = 0;
	queue.m_read = queue.m_write = queue.m_entries;
	queue.m_entries[0].m_wordIndex = 0;
	queue.m_entries[0].m_scale = 1;
	queue.m_entries[0].m_center = p_tree->m_center;
	queue.AdvanceWrite();
	while (queue.m_readCount != queue.m_writeCount) {
		CollisionTreeQueueEntry* current = queue.m_read;
		const LegoU16* words = p_tree->m_words + current->m_wordIndex;
		if (*words == 0) {
			LegoS32 count = words[1];
			if (candidates.m_count + count > candidates.m_capacity) {
				count = candidates.m_capacity - candidates.m_count;
			}
			if (count > 0) {
				CollisionTriangleIndexNode* previous = (CollisionTriangleIndexNode*) &candidates;
				if (candidates.m_count == 0) {
					candidates.AppendRange(words + 2, count, &previous);
				}
				else {
					for (LegoS32 i = 0; i < count; ++i) {
						candidates.Insert(words[2 + i], &previous);
					}
				}
			}
		}
		else {
			if ((LegoS32) (queue.m_writeCount - queue.m_readCount + 8) > 256) {
				return;
			}
			LegoFloat scale = 1.0f / (LegoFloat) ((LegoU32) current->m_scale << 1);
			Vector3 halfExtent;
			// The original helper leaves unsplit components untouched. This
			// remains unresolved; retain its writes rather than invent defaults.
			GetCollisionChildHalfExtent(p_tree, NULL, scale, &halfExtent);
			const LegoU16* child = words + 1;
			for (LegoS32 i = 0; i < p_tree->m_childCount; ++i) {
				if (((1u << i) & *words) != 0) {
					CollisionTreeQueueEntry* output = queue.m_write;
					output->m_center = current->m_center;
					output->m_center.m_x += scale * p_tree->m_childOffsets[i].m_x;
					output->m_center.m_y += scale * p_tree->m_childOffsets[i].m_y;
					output->m_center.m_z += scale * p_tree->m_childOffsets[i].m_z;
					if (IntersectSegmentBounds(p_start, p_delta, &distance, &output->m_center, &halfExtent)) {
						output->m_scale = (LegoU16) ((LegoU32) current->m_scale << 1);
						output->m_wordIndex = (LegoU16) (*child + current->m_wordIndex);
						queue.AdvanceWrite();
					}
					++child;
				}
			}
		}
		++queue.m_readCount;
		queue.m_read = &queue.m_entries[queue.m_readCount & 0xff];
	}
}

// STUB: LEGORACERS2 0x0050b380
static void __fastcall GetCollisionChildHalfExtent(
	CollisionTriangleTree* p_tree,
	void*,
	LegoFloat p_scale,
	Vector3* p_output
)
{
	if (p_tree->m_splitAxes > 7) {
		return;
	}
	if (p_tree->m_splitAxes == 0) {
		*p_output = p_tree->m_halfExtent;
		return;
	}
	if (p_tree->m_splitAxes & 4) {
		p_output->m_x = p_scale * p_tree->m_halfExtent.m_x;
	}
	if (p_tree->m_splitAxes & 2) {
		p_output->m_y = p_scale * p_tree->m_halfExtent.m_y;
	}
	if (p_tree->m_splitAxes & 1) {
		p_output->m_z = p_scale * p_tree->m_halfExtent.m_z;
	}
}
