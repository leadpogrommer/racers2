#include "physics/collision/collisionspatialindex.h"

SpatialSlotNode* __fastcall InsertSpatialAxis(SpatialAxisIndex* p_this, void*, void* p_value)
{
	return ((SpatialLinkRegistry*) p_this)->InsertValue(p_value);
}

void __fastcall UpdateSpatialXAxis(SpatialAxisIndex* p_this, void*, void* p_node)
{
	p_this->UpdateFirst((SpatialSlotNode*) p_node);
}

void __fastcall UpdateSpatialZAxis(SpatialAxisIndex* p_this, void*, void* p_node)
{
	p_this->UpdateSecond((SpatialSlotNode*) p_node);
}

// Complete retail tables 0x005375cc and 0x005375d4. Following words are floats.
// GLOBAL: LEGORACERS2 0x005375cc
extern const SpatialAxisDispatch g_spatialXAxisDispatch = {InsertSpatialAxis, UpdateSpatialXAxis};
// GLOBAL: LEGORACERS2 0x005375d4
extern const SpatialAxisDispatch g_spatialZAxisDispatch = {InsertSpatialAxis, UpdateSpatialZAxis};

static void SwapSpatialAxisValues(SpatialSlotNode* p_first, SpatialSlotNode* p_second, LegoBool32 p_zAxis)
{
	// The original uses XOR swaps, including their behavior on aliased handles.
	LegoU32* first = (LegoU32*) &p_first->m_value;
	LegoU32* second = (LegoU32*) &p_second->m_value;
	*first ^= *second;
	*second ^= *first;
	*first ^= *second;
	SpatialIndexLink* firstLink = (SpatialIndexLink*) p_first->m_value;
	SpatialIndexLink* secondLink = (SpatialIndexLink*) p_second->m_value;
	first = (LegoU32*) (p_zAxis ? &firstLink->m_zHandle : &firstLink->m_xHandle);
	second = (LegoU32*) (p_zAxis ? &secondLink->m_zHandle : &secondLink->m_xHandle);
	*first ^= *second;
	*second ^= *first;
	*first ^= *second;
}

static LegoFloat GetSpatialAxisMinimum(SpatialSlotNode* p_node, LegoBool32 p_zAxis)
{
	SpatialIndexLink* link = (SpatialIndexLink*) p_node->m_value;
	return p_zAxis ? link->m_minZ : link->m_minX;
}

static void UpdateSpatialAxisNode(SpatialSlotNode* p_node, LegoBool32 p_zAxis)
{
	SpatialSlotNode* next = p_node->m_next;
	if (next != NULL && GetSpatialAxisMinimum(next, p_zAxis) < GetSpatialAxisMinimum(p_node, p_zAxis)) {
		do {
			SwapSpatialAxisValues(p_node, next, p_zAxis);
			p_node = next;
			next = p_node->m_next;
			if (next == NULL) {
				return;
			}
		} while (GetSpatialAxisMinimum(next, p_zAxis) < GetSpatialAxisMinimum(p_node, p_zAxis));
	}
	else {
		SpatialSlotNode* previous = p_node->m_previous;
		while (previous != NULL && GetSpatialAxisMinimum(p_node, p_zAxis) < GetSpatialAxisMinimum(previous, p_zAxis)) {
			SwapSpatialAxisValues(previous, p_node, p_zAxis);
			p_node = previous;
			previous = p_node->m_previous;
		}
	}
}

static void InsertSpatialAxisLink(SpatialAxisIndex* p_this, SpatialIndexLink* p_link, LegoBool32 p_zAxis)
{
	if (p_this->m_head == NULL) {
		p_this->m_head = p_this->m_allocator->AllocateBefore(p_link, NULL);
		if (p_zAxis) {
			p_link->m_zHandle = p_this->m_head;
		}
		else {
			p_link->m_xHandle = p_this->m_head;
		}
		return;
	}
	p_this->m_head = p_this->m_allocator->AllocateBefore(p_link, p_this->m_head);
	if (p_zAxis) {
		p_link->m_zHandle = p_this->m_head;
	}
	else {
		p_link->m_xHandle = p_this->m_head;
	}
	SpatialSlotNode* current = p_this->m_head;
	SpatialSlotNode* next = current->m_next;
	// The initial comparison includes equality; later comparisons are strict.
	if (GetSpatialAxisMinimum(next, p_zAxis) <= GetSpatialAxisMinimum(current, p_zAxis)) {
		do {
			SwapSpatialAxisValues(next, current, p_zAxis);
			current = next;
			next = current->m_next;
			if (next == NULL) {
				return;
			}
		} while (GetSpatialAxisMinimum(next, p_zAxis) < GetSpatialAxisMinimum(current, p_zAxis));
	}
}

// FUNCTION: LEGORACERS2 0x00507ca0
void SpatialAxisIndex::Remove(void* p_handle)
{
	((SpatialLinkRegistry*) this)->RemoveNode((SpatialSlotNode*) p_handle);
}

// STUB: LEGORACERS2 0x00508010
void SpatialAxisIndex::UpdateFirst(SpatialSlotNode* p_node)
{
	UpdateSpatialAxisNode(p_node, FALSE);
}

// STUB: LEGORACERS2 0x00508100
void SpatialAxisIndex::UpdateSecond(SpatialSlotNode* p_node)
{
	UpdateSpatialAxisNode(p_node, TRUE);
}

// STUB: LEGORACERS2 0x00508330
void SpatialAxisIndex::InsertFirst(SpatialIndexLink* p_link)
{
	InsertSpatialAxisLink(this, p_link, FALSE);
}

// STUB: LEGORACERS2 0x005083e0
void SpatialAxisIndex::InsertSecond(SpatialIndexLink* p_link)
{
	InsertSpatialAxisLink(this, p_link, TRUE);
}
