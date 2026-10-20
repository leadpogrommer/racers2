#include "physics/collision/collisionspatialindex.h"

#include "memory/memoryheap.h"

#include <new.h>
#include <string.h>

// STUB: LEGORACERS2 0x004f51b0
CollisionSpatialIndex::CollisionSpatialIndex(
	LegoU32 p_size,
	const Vector3* p_origin,
	LegoFloat p_scale,
	LegoU32 p_capacity,
	LegoU32 p_staticCapacity,
	LegoU32 p_unused
)
{
	void* memory = AllocateHeapMemory(0x10);
	m_allocator = memory == NULL ? NULL : new (memory) SpatialSlotAllocator(p_capacity);
	memory = AllocateHeapMemory(0x10);
	m_registry = memory == NULL ? NULL : new (memory) SpatialLinkRegistry(m_allocator);
	memory = AllocateHeapMemory(0x24);
	m_staticGrid = memory == NULL ? NULL
								  : new (memory)
										StaticSpatialGrid(p_size, p_origin, p_scale, p_staticCapacity, m_allocator);
	memory = AllocateHeapMemory(0x70);
	m_dynamicIndex = memory == NULL ? NULL : new (memory) DynamicSpatialIndex(512, m_allocator, m_registry);
}

// FUNCTION: LEGORACERS2 0x004f5260
LegoS32 CollisionSpatialIndex::InsertStatic(SpatialIndexLink* p_link)
{
	LegoS32 index = m_registry->Insert(p_link);
	if (index != -1) {
		m_staticGrid->Insert(p_link);
	}
	return index;
}

// FUNCTION: LEGORACERS2 0x004f5290
LegoS32 CollisionSpatialIndex::InsertDynamic(SpatialIndexLink* p_link)
{
	LegoS32 index = m_registry->Insert(p_link);
	if (index != -1) {
		DynamicSpatialIndex* dynamicIndex = m_dynamicIndex;
		dynamicIndex->m_axes[0].InsertFirst(p_link);
		dynamicIndex->m_axes[1].InsertSecond(p_link);
	}
	return index;
}

// FUNCTION: LEGORACERS2 0x004f52d0
LegoS32 CollisionSpatialIndex::RemoveStatic(SpatialIndexLink* p_link)
{
	m_staticGrid->Remove(p_link);
	return m_registry->Remove(p_link);
}

// FUNCTION: LEGORACERS2 0x004f52f0
LegoS32 CollisionSpatialIndex::RemoveDynamic(SpatialIndexLink* p_link)
{
	DynamicSpatialIndex* dynamicIndex = m_dynamicIndex;
	if (p_link->m_xHandle != NULL) {
		dynamicIndex->m_axes[0].Remove(p_link->m_xHandle);
	}
	if (p_link->m_zHandle != NULL) {
		dynamicIndex->m_axes[1].Remove(p_link->m_zHandle);
	}
	return m_registry->Remove(p_link);
}

// FUNCTION: LEGORACERS2 0x004f5330
void CollisionSpatialIndex::UpdateStatic(SpatialIndexLink* p_link)
{
	m_staticGrid->Update(p_link);
}

// STUB: LEGORACERS2 0x004f5340
void CollisionSpatialIndex::UpdateDynamic(SpatialIndexLink* p_link)
{
	DynamicSpatialIndex* dynamicIndex = m_dynamicIndex;
	SpatialAxisIndex* first = &dynamicIndex->m_axes[0];
	first->m_vtable->m_update(first, NULL, p_link->m_xHandle);
	SpatialAxisIndex* second = &dynamicIndex->m_axes[1];
	second->m_vtable->m_update(second, NULL, p_link->m_zHandle);
}

// STUB: LEGORACERS2 0x004fb690
void SpatialIndexLink::Detach()
{
	if (m_index != -1) {
		LegoS32 index;
		if (m_filter == 0) {
			index = g_collisionSpatialIndex->RemoveStatic(this);
		}
		else {
			index = g_collisionSpatialIndex->RemoveDynamic(this);
		}
		memset(m_unk0x0c, 0, sizeof(*this) - offsetof(SpatialIndexLink, m_unk0x0c));
		m_index = index;
	}
}

// STUB: LEGORACERS2 0x004fb6e0
void SpatialIndexLink::RefreshBounds()
{
	m_minX = m_proxy->m_position.m_x - m_proxy->m_model->m_radius;
	m_minZ = m_proxy->m_position.m_z - m_proxy->m_model->m_radius;
	m_maxX = m_proxy->m_position.m_x + m_proxy->m_model->m_radius;
	m_maxZ = m_proxy->m_position.m_z + m_proxy->m_model->m_radius;
}

// FUNCTION: LEGORACERS2 0x004fb710
void SpatialIndexLink::Attach()
{
	if (m_index == -1) {
		RefreshBounds();
		if (m_filter != 0) {
			m_index = g_collisionSpatialIndex->InsertDynamic(this);
		}
		else {
			m_index = g_collisionSpatialIndex->InsertStatic(this);
		}
	}
}

// STUB: LEGORACERS2 0x004fb740
void SpatialIndexLink::SetFilter(const LegoS32* p_filter)
{
	if (m_index != -1 && ((*p_filter == 0) != (m_filter == 0))) {
		Detach();
		m_filter = *p_filter;
		Attach();
	}
	m_filter = *p_filter;
}

// FUNCTION: LEGORACERS2 0x004fb7a0
void SpatialIndexLink::Update()
{
	if (m_index != -1) {
		RefreshBounds();
		if (m_filter != 0) {
			g_collisionSpatialIndex->UpdateDynamic(this);
		}
		else {
			g_collisionSpatialIndex->UpdateStatic(this);
		}
	}
}
