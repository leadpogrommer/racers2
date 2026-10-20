#include "physics/collision/collisionspatialindex.h"

SpatialSlotNode* __fastcall InsertSpatialList(SpatialLinkRegistry* p_this, void*, void* p_value)
{
	return p_this->InsertValue(p_value);
}

void __fastcall UpdateSpatialList(SpatialLinkRegistry* p_this, void*, SpatialSlotNode* p_node)
{
	p_this->UpdateNode(p_node);
}

// Both retail tables contain the same two callbacks; following words are data.
// Retail table: 0x00537084; verified separately from method-body annotations.
// GLOBAL: LEGORACERS2 0x00537084
extern const SpatialListDispatch g_spatialRegistryDispatch = {InsertSpatialList, UpdateSpatialList};

// Retail table: 0x00537528; verified separately from method-body annotations.
// GLOBAL: LEGORACERS2 0x00537528
extern const SpatialListDispatch g_spatialListDispatch = {InsertSpatialList, UpdateSpatialList};

// FUNCTION: LEGORACERS2 0x004f5140
SpatialLinkRegistry::SpatialLinkRegistry(SpatialSlotAllocator* p_allocator)
{
	InitializeList(p_allocator, NULL);
	m_vtable = &g_spatialRegistryDispatch;
}

// STUB: LEGORACERS2 0x004f5160
LegoS32 SpatialLinkRegistry::Insert(SpatialIndexLink* p_link)
{
	return m_allocator->GetNodeIndex(InsertValue(p_link));
}

// FUNCTION: LEGORACERS2 0x004f5180
LegoS32 SpatialLinkRegistry::Remove(SpatialIndexLink* p_link)
{
	RemoveValue(p_link);
	return -1;
}

// STUB: LEGORACERS2 0x005075a0
SpatialLinkRegistry* SpatialLinkRegistry::InitializeList(
	SpatialSlotAllocator* p_allocator,
	SpatialValueCompare p_compare
)
{
	m_compare = p_compare;
	m_allocator = p_allocator;
	m_vtable = &g_spatialListDispatch;
	ClearHead();
	return this;
}

// FUNCTION: LEGORACERS2 0x005075d0
void SpatialLinkRegistry::ClearHead()
{
	m_head = NULL;
}

// STUB: LEGORACERS2 0x005075e0
SpatialSlotNode* SpatialLinkRegistry::InsertValue(void* p_value)
{
	if (m_compare != NULL && m_head != NULL) {
		if (!m_compare(m_head->m_value, p_value)) {
			SpatialSlotNode* previous = m_head;
			SpatialSlotNode* next = m_head->m_next;
			while (next != NULL && !m_compare(next->m_value, p_value)) {
				previous = next;
				next = next->m_next;
			}
			return m_allocator->AllocateAfter(p_value, previous);
		}
	}
	m_head = m_allocator->AllocateBefore(p_value, m_head);
	return m_head;
}

// STUB: LEGORACERS2 0x00507670
void SpatialLinkRegistry::UpdateNode(SpatialSlotNode* p_node)
{
	if (m_compare == NULL) {
		return;
	}
	SpatialSlotNode* neighbor = p_node->m_next;
	if (neighbor != NULL && m_compare(neighbor->m_value, p_node->m_value)) {
		do {
			neighbor->m_previous = p_node->m_previous;
			p_node->m_next = neighbor->m_next;
			neighbor->m_next = p_node;
			p_node->m_previous = neighbor;
			// Preserve the self-link write present in the retail instructions.
			if (neighbor != NULL) {
				neighbor->m_next = neighbor;
			}
			neighbor = p_node->m_next;
			if (neighbor == NULL) {
				return;
			}
			neighbor->m_previous = p_node;
		} while (m_compare(neighbor->m_value, p_node->m_value));
	}
	else {
		neighbor = p_node->m_previous;
		while (neighbor != NULL && m_compare(p_node->m_value, neighbor->m_value)) {
			p_node->m_previous = neighbor->m_previous;
			neighbor->m_next = p_node->m_next;
			neighbor->m_previous = p_node;
			p_node->m_next = neighbor;
			if (neighbor != NULL) {
				neighbor->m_previous = neighbor;
			}
			neighbor = p_node->m_previous;
			if (neighbor == NULL) {
				return;
			}
			neighbor->m_next = p_node;
		}
	}
}

// FUNCTION: LEGORACERS2 0x00507720
void SpatialLinkRegistry::RemoveValue(void* p_value)
{
	m_allocator->RemoveValue(p_value, &m_head);
}

// STUB: LEGORACERS2 0x00507740
void SpatialLinkRegistry::RemoveNode(SpatialSlotNode* p_node)
{
	if (m_head == NULL) {
		return;
	}
	if (p_node == m_head) {
		m_head = m_head->m_next;
	}
	m_allocator->ReleaseNode(&p_node);
}

// FUNCTION: LEGORACERS2 0x00507770
LegoS32 SpatialLinkRegistry::GetNodeIndex(SpatialSlotNode* p_node)
{
	return m_allocator->GetNodeIndex(p_node);
}

// FUNCTION: LEGORACERS2 0x00507780
void* SpatialLinkRegistry::GetNodeValue(LegoS32 p_index)
{
	return m_allocator->GetNodeValue(p_index);
}
