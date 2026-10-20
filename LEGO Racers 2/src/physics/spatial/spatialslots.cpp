#include "memory/memoryheap.h"
#include "physics/collision/collisionspatialindex.h"

// STUB: LEGORACERS2 0x00507310
void SpatialSlotNode::Initialize()
{
	m_value = NULL;
	m_next = NULL;
	m_previous = NULL;
}

// STUB: LEGORACERS2 0x00507320
void SpatialSlotNode::InsertBefore(void* p_value, SpatialSlotNode* p_next)
{
	m_value = p_value;
	m_next = p_next;
	m_previous = p_next == NULL ? NULL : p_next->m_previous;
	if (m_next != NULL) {
		m_next->m_previous = this;
	}
	if (m_previous != NULL) {
		m_previous->m_next = this;
	}
}

// STUB: LEGORACERS2 0x00507350
void SpatialSlotNode::InsertAfter(void* p_value, SpatialSlotNode* p_previous)
{
	m_value = p_value;
	m_previous = p_previous;
	m_next = p_previous == NULL ? NULL : p_previous->m_next;
	if (m_previous != NULL) {
		m_previous->m_next = this;
	}
	if (m_next != NULL) {
		m_next->m_previous = this;
	}
}

// FUNCTION: LEGORACERS2 0x00507380
void SpatialSlotNode::Unlink()
{
	if (m_previous != NULL) {
		m_previous->m_next = m_next;
	}
	if (m_next != NULL) {
		m_next->m_previous = m_previous;
	}
}

// STUB: LEGORACERS2 0x005073a0
SpatialSlotAllocator::SpatialSlotAllocator(LegoS32 p_capacity)
{
	m_capacity = p_capacity;
	m_nodes = (SpatialSlotNode*) AllocateHeapMemory(p_capacity * sizeof(SpatialSlotNode));
	if (m_nodes != NULL) {
		for (LegoS32 i = 0; i < p_capacity; ++i) {
			m_nodes[i].Initialize();
		}
	}
	Reset();
}

// FUNCTION: LEGORACERS2 0x00507410
void SpatialSlotAllocator::Reset()
{
	m_free = NULL;
	m_count = 0;
}

// STUB: LEGORACERS2 0x00507420
SpatialSlotNode* SpatialSlotAllocator::AllocateBefore(void* p_value, SpatialSlotNode* p_next)
{
	SpatialSlotNode* node = m_free;
	if (node == NULL) {
		if (m_capacity <= m_count) {
			return NULL;
		}
		node = &m_nodes[m_count];
	}
	else {
		m_free = node->m_next;
	}
	++m_count;
	node->InsertBefore(p_value, p_next);
	return node;
}

static SpatialSlotNode* __stdcall FindSpatialSlot(void* p_value, SpatialSlotNode* p_head);

// STUB: LEGORACERS2 0x00507470
void SpatialSlotAllocator::RemoveValue(void* p_value, SpatialSlotNode** p_head)
{
	SpatialSlotNode* node = FindSpatialSlot(p_value, *p_head);
	if (node == NULL) {
		return;
	}
	if (node == *p_head) {
		*p_head = node->m_next;
	}
	ReleaseNode(&node);
}

// FUNCTION: LEGORACERS2 0x005074b0
LegoBool32 SpatialSlotAllocator::ReleaseNode(SpatialSlotNode** p_node)
{
	if (*p_node != NULL) {
		(*p_node)->Unlink();
	}
	// The retail function also dereferences a null input after the conditional.
	(*p_node)->m_next = m_free;
	m_free = *p_node;
	if (--m_count == 0) {
		m_free = NULL;
	}
	*p_node = NULL;
	return TRUE;
}

// FUNCTION: LEGORACERS2 0x005074f0
LegoS32 SpatialSlotAllocator::GetNodeIndex(SpatialSlotNode* p_node)
{
	return (LegoS32) ((LegoU32) p_node - (LegoU32) m_nodes) / (LegoS32) sizeof(SpatialSlotNode);
}

// FUNCTION: LEGORACERS2 0x00507510
SpatialSlotNode* SpatialSlotAllocator::GetNodeAtIndex(LegoS32 p_index)
{
	return &m_nodes[p_index];
}

// FUNCTION: LEGORACERS2 0x00507520
void* SpatialSlotAllocator::GetNodeValue(LegoS32 p_index)
{
	return GetNodeAtIndex(p_index)->m_value;
}

// FUNCTION: LEGORACERS2 0x00507530
static SpatialSlotNode* __stdcall FindSpatialSlot(void* p_value, SpatialSlotNode* p_head)
{
	while (p_head != NULL) {
		if (p_head->m_value == p_value) {
			return p_head;
		}
		p_head = p_head->m_next;
	}
	return NULL;
}

// STUB: LEGORACERS2 0x00507550
SpatialSlotNode* SpatialSlotAllocator::AllocateAfter(void* p_value, SpatialSlotNode* p_previous)
{
	SpatialSlotNode* node = m_free;
	if (node == NULL) {
		if (m_capacity <= m_count) {
			return NULL;
		}
		node = &m_nodes[m_count];
	}
	else {
		m_free = node->m_next;
	}
	++m_count;
	node->InsertAfter(p_value, p_previous);
	return node;
}
