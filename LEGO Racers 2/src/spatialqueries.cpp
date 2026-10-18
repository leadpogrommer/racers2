#include "collisionqueries.h"

// The retail filter matrix spans [-4, 1] on both axes. Equal original
// filters are rejected before clamping, even when their clamped values match.
static LegoBool32 CanPairFilters(LegoS32 p_first, LegoS32 p_second)
{
	static const LegoU32 filters[6][6] = {
		{0, 0, 1, 0, 1, 0},
		{0, 0, 0, 1, 0, 1},
		{1, 0, 0, 1, 0, 1},
		{0, 1, 1, 0, 0, 1},
		{1, 0, 0, 0, 0, 1},
		{0, 1, 1, 1, 1, 1}
	};
	if (p_first == p_second) {
		return FALSE;
	}
	if (p_first < -4) {
		p_first = -4;
	}
	else if (p_first > 1) {
		p_first = 1;
	}
	if (p_second < -4) {
		p_second = -4;
	}
	else if (p_second > 1) {
		p_second = 1;
	}
	return filters[p_first + 4][p_second + 4];
}

static void BuildAxisPairs(SpatialAxisIndex*, SpatialPairList*, LegoBool32);

// STUB: LEGORACERS2 0x00507910
LegoBool32 SpatialPairList::AddPair(LegoS32 p_first, LegoS32 p_second)
{
	if (!CanPair(p_first, p_second)) {
		return TRUE;
	}
	if (p_second < p_first) {
		LegoS32 temporary = p_first;
		p_first = p_second;
		p_second = temporary;
	}
	LegoU32 key = (LegoU16) p_first | ((LegoU32) (LegoU16) p_second << 16);
	LegoU32* previous = &m_headIndex;
	LegoU32 index = *previous;
	while (index != 0xffff && (LegoS32) m_nodes[index].m_value < (LegoS32) key) {
		previous = &m_nodes[index].m_next;
		index = *previous;
	}
	if (index != 0xffff && m_nodes[index].m_value == key && m_unk0x18 != 0) {
		return FALSE;
	}
	if (m_count >= m_capacity) {
		return FALSE;
	}
	m_nodes[m_count].m_next = *previous;
	*previous = m_count;
	m_nodes[m_count].m_value = key;
	++m_count;
	return TRUE;
}

// STUB: LEGORACERS2 0x00507a00
LegoBool32 SpatialPairList::CanPair(LegoS32 p_first, LegoS32 p_second)
{
	SpatialIndexLink* second = (SpatialIndexLink*) m_registry->GetNodeValue(p_second);
	SpatialIndexLink* first = (SpatialIndexLink*) m_registry->GetNodeValue(p_first);
	return CanPairFilters(first->m_filter, second->m_filter);
}

// STUB: LEGORACERS2 0x00507a80
void SpatialPairList::ProcessCommonPairs(const SpatialPairList* p_other)
{
	if (m_count == 0 || p_other->m_count == 0) {
		return;
	}
	LegoU32 second = p_other->m_headIndex;
	for (LegoU32 first = m_headIndex; first != 0xffff; first = m_nodes[first].m_next) {
		LegoU32 key = m_nodes[first].m_value;
		while ((LegoS32) p_other->m_nodes[second].m_value < (LegoS32) key) {
			second = p_other->m_nodes[second].m_next;
			if (second == 0xffff) {
				return;
			}
		}
		if (key == p_other->m_nodes[second].m_value) {
			SpatialIndexLink* firstLink = (SpatialIndexLink*) m_registry->GetNodeValue(key & 0xffff);
			SpatialIndexLink* secondLink = (SpatialIndexLink*) m_registry->GetNodeValue(key >> 16);
			TryRecordCollisionPair(g_collisionServiceState, NULL, firstLink->m_proxy, secondLink->m_proxy);
		}
	}
}

void __fastcall BuildXAxisCollisionPairs(SpatialAxisIndex*, void*, SpatialPairList*);
void __fastcall BuildZAxisCollisionPairs(SpatialAxisIndex*, void*, SpatialPairList*);

// STUB: LEGORACERS2 0x00507d60
void DynamicSpatialIndex::ResetQuery()
{
	BuildXAxisCollisionPairs(&m_axes[0], NULL, &m_xPairs);
	BuildZAxisCollisionPairs(&m_axes[1], NULL, &m_zPairs);
	m_xPairs.ProcessCommonPairs(&m_zPairs);
}

// STUB: LEGORACERS2 0x00507dc0
static LegoU32 __cdecl IntersectCollisionCandidateArrays(
	LegoU32* p_output,
	const LegoU32* p_first,
	const LegoU32* p_firstEnd,
	const LegoU32* p_second,
	const LegoU32* p_secondEnd
)
{
	LegoU32* output = p_output;
	for (; p_first != p_firstEnd; ++p_first) {
		for (const LegoU32* second = p_second; second != p_secondEnd; ++second) {
			if (*p_first == *second) {
				*output++ = *p_first;
				break;
			}
		}
	}
	return output - p_output;
}

LegoU32 __fastcall CollectXAxisCandidates(SpatialAxisIndex*, void*, const CollisionQueryBounds*, LegoU32*, LegoU32);
LegoU32 __fastcall CollectZAxisCandidates(SpatialAxisIndex*, void*, const CollisionQueryBounds*, LegoU32*, LegoU32);

// STUB: LEGORACERS2 0x00507e10
LegoU32 DynamicSpatialIndex::CollectCandidates(const CollisionQueryBounds* p_bounds, LegoU32* p_tags)
{
	LegoU32 capacity = p_tags[0];
	LegoU32 xCount = CollectXAxisCandidates(&m_axes[0], NULL, p_bounds, p_tags, capacity);
	LegoU32* zTags = p_tags + xCount;
	LegoU32 zCount = CollectZAxisCandidates(&m_axes[1], NULL, p_bounds, zTags, capacity - xCount);
	return IntersectCollisionCandidateArrays(p_tags, p_tags, zTags, zTags, zTags + zCount);
}

// STUB: LEGORACERS2 0x00507ef0
void __fastcall BuildXAxisCollisionPairs(SpatialAxisIndex* p_axis, void*, SpatialPairList* p_pairs)
{
	BuildAxisPairs(p_axis, p_pairs, FALSE);
}

// STUB: LEGORACERS2 0x00507f80
void __fastcall BuildZAxisCollisionPairs(SpatialAxisIndex* p_axis, void*, SpatialPairList* p_pairs)
{
	BuildAxisPairs(p_axis, p_pairs, TRUE);
}

static void BuildAxisPairs(SpatialAxisIndex* p_axis, SpatialPairList* p_pairs, LegoBool32 p_zAxis)
{
	p_pairs->m_count = 0;
	p_pairs->m_headIndex = 0xffff;
	for (SpatialSlotNode* first = p_axis->m_head; first != NULL; first = first->m_next) {
		SpatialIndexLink* firstLink = (SpatialIndexLink*) first->m_value;
		LegoFloat maximum = p_zAxis ? firstLink->m_maxZ : firstLink->m_maxX;
		for (SpatialSlotNode* second = first->m_next; second != NULL; second = second->m_next) {
			SpatialIndexLink* secondLink = (SpatialIndexLink*) second->m_value;
			LegoFloat minimum = p_zAxis ? secondLink->m_minZ : secondLink->m_minX;
			if (!(minimum <= maximum)) {
				break;
			}
			if (CanPairFilters(firstLink->m_filter, secondLink->m_filter) &&
				!p_pairs->AddPair(firstLink->m_index, secondLink->m_index)) {
				return;
			}
		}
	}
}

static LegoU32 CollectAxisCandidates(
	SpatialAxisIndex* p_axis,
	const CollisionQueryBounds* p_bounds,
	LegoU32* p_tags,
	LegoU32 p_capacity,
	LegoBool32 p_zAxis
)
{
	LegoU32* output = p_tags;
	for (SpatialSlotNode* node = p_axis->m_head; node != NULL; node = node->m_next) {
		SpatialIndexLink* link = (SpatialIndexLink*) node->m_value;
		LegoFloat minimum = p_zAxis ? link->m_minZ : link->m_minX;
		LegoFloat maximum = p_zAxis ? link->m_maxZ : link->m_maxX;
		if ((p_zAxis ? p_bounds->m_maxZ : p_bounds->m_maxX) < minimum) {
			break;
		}
		if ((p_zAxis ? p_bounds->m_minZ : p_bounds->m_minX) <= maximum) {
			if (output == p_tags + p_capacity) {
				break;
			}
			SpatialIndexLink* registered = (SpatialIndexLink*) p_axis->m_registry->GetNodeValue(link->m_index);
			*output++ = registered->m_proxy->m_tag;
		}
	}
	return output - p_tags;
}

// STUB: LEGORACERS2 0x005081f0
LegoU32 __fastcall CollectZAxisCandidates(
	SpatialAxisIndex* p_axis,
	void*,
	const CollisionQueryBounds* p_bounds,
	LegoU32* p_tags,
	LegoU32 p_capacity
)
{
	return CollectAxisCandidates(p_axis, p_bounds, p_tags, p_capacity, TRUE);
}

// STUB: LEGORACERS2 0x00508290
LegoU32 __fastcall CollectXAxisCandidates(
	SpatialAxisIndex* p_axis,
	void*,
	const CollisionQueryBounds* p_bounds,
	LegoU32* p_tags,
	LegoU32 p_capacity
)
{
	return CollectAxisCandidates(p_axis, p_bounds, p_tags, p_capacity, FALSE);
}
