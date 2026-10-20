#include "memory/memoryheap.h"
#include "physics/collision/collisionspatialindex.h"

// Complete two-slot axis tables are emitted with the corresponding update bodies.
extern const SpatialAxisDispatch g_spatialXAxisDispatch;
extern const SpatialAxisDispatch g_spatialZAxisDispatch;

// STUB: LEGORACERS2 0x005078c0
SpatialPairList* SpatialPairList::Initialize(LegoU32 p_capacity, SpatialLinkRegistry* p_registry)
{
	m_capacity = p_capacity;
	m_nodes = (SpatialPairNode*) GetMemoryHeap()->Allocate(m_capacity << 3, "New scList32");
	m_headIndex = 0xffff;
	// This fixed sentinel offset is present even when capacity is 512.
	m_tail = (SpatialPairNode*) ((char*) m_nodes + 0x7fff8);
	m_unk0x18 = 0;
	m_count = 0;
	m_registry = p_registry;
	return this;
}

// STUB: LEGORACERS2 0x00507be0
SpatialAxisIndex* SpatialAxisIndex::InitializeFirst(SpatialLinkRegistry* p_registry, SpatialSlotAllocator* p_allocator)
{
	((SpatialLinkRegistry*) this)->InitializeList(p_allocator, NULL);
	m_vtable = &g_spatialXAxisDispatch;
	m_registry = p_registry;
	return this;
}

// STUB: LEGORACERS2 0x00507cb0
SpatialAxisIndex* SpatialAxisIndex::InitializeSecond(SpatialLinkRegistry* p_registry, SpatialSlotAllocator* p_allocator)
{
	((SpatialLinkRegistry*) this)->InitializeList(p_allocator, NULL);
	m_vtable = &g_spatialZAxisDispatch;
	m_registry = p_registry;
	return this;
}

// FUNCTION: LEGORACERS2 0x00507d10
DynamicSpatialIndex::DynamicSpatialIndex(
	LegoU32 p_capacity,
	SpatialSlotAllocator* p_allocator,
	SpatialLinkRegistry* p_registry
)
{
	m_axes[0].InitializeFirst(p_registry, p_allocator);
	m_axes[1].InitializeSecond(p_registry, p_allocator);
	m_xPairs.Initialize(p_capacity, p_registry);
	m_zPairs.Initialize(p_capacity, p_registry);
	m_registry = p_registry;
	m_allocator = p_allocator;
}
