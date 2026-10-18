#include "collisionspatialindex.h"
#include "memoryheap.h"

static LegoS32 RoundGridCoordinate(LegoFloat p_value)
{
	volatile LegoFloat storedValue = p_value;
	LegoS32 result;
	__asm {
		fld storedValue
		fistp result
	}
	return result;
}

// STUB: LEGORACERS2 0x004f90e0
LegoS32 __cdecl GetSpatialGridShift(LegoS32 p_size)
{
	LegoS32 result = -1;
	for (LegoS32 shift = 0; shift < 32; ++shift) {
		if (p_size == 1) {
			result = shift;
		}
		p_size >>= 1;
	}
	return result;
}

// STUB: LEGORACERS2 0x005084b0
void SpatialGridCell::Initialize()
{
	m_head = NULL;
}

// FUNCTION: LEGORACERS2 0x005084c0
void SpatialGridCell::Insert(void* p_value, SpatialSlotAllocator* p_allocator)
{
	m_head = p_allocator->AllocateBefore(p_value, m_head);
}

// FUNCTION: LEGORACERS2 0x005084e0
void SpatialGridCell::Remove(void* p_value, SpatialSlotAllocator* p_allocator)
{
	p_allocator->RemoveValue(p_value, &m_head);
}

// STUB: LEGORACERS2 0x00508500
SpatialGridCell* StaticSpatialGrid::GetCell(LegoS32 p_x, LegoS32 p_z)
{
	return &m_cells[((LegoU32) p_z << (m_shift & 31)) + p_x];
}

// STUB: LEGORACERS2 0x00508690
StaticSpatialGrid::StaticSpatialGrid(
	LegoU32 p_size,
	const Vector3* p_origin,
	LegoFloat p_scale,
	LegoU32 p_capacity,
	SpatialSlotAllocator* p_allocator
)
{
	m_size = p_size;
	m_diameter = p_scale + p_scale;
	m_inverseCellSize = (LegoS32) p_size / m_diameter;
	m_origin = *p_origin;
	m_origin.m_x -= p_scale;
	m_origin.m_z -= p_scale;
	m_shift = GetSpatialGridShift(p_size);
	m_allocator = p_allocator;
	LegoS32 count = m_size * m_size;
	m_cells = (SpatialGridCell*) AllocateHeapMemory(count * sizeof(SpatialGridCell));
	if (m_cells != NULL) {
		for (LegoS32 i = 0; i < count; ++i) {
			m_cells[i].Initialize();
		}
	}
}

// STUB: LEGORACERS2 0x00508740
void StaticSpatialGrid::Insert(SpatialIndexLink* p_link)
{
	LegoFloat inverse = m_inverseCellSize;
	LegoS32 size = m_size;
	p_link->m_gridMinX = RoundGridCoordinate((p_link->m_minX - m_origin.m_x) * inverse - 0.5f);
	p_link->m_gridMinZ = RoundGridCoordinate((p_link->m_minZ - m_origin.m_z) * inverse - 0.5f);
	p_link->m_gridMaxX = RoundGridCoordinate((p_link->m_maxX - m_origin.m_x) * inverse + 0.5f);
	p_link->m_gridMaxZ = RoundGridCoordinate((p_link->m_maxZ - m_origin.m_z) * inverse + 0.5f);
	if (p_link->m_gridMinX < 0) {
		p_link->m_gridMinX = 0;
	}
	if (p_link->m_gridMinZ < 0) {
		p_link->m_gridMinZ = 0;
	}
	if (p_link->m_gridMaxX >= size) {
		p_link->m_gridMaxX = size - 1;
	}
	if (p_link->m_gridMaxZ >= size) {
		p_link->m_gridMaxZ = size - 1;
	}
	for (LegoS32 x = p_link->m_gridMinX; x < p_link->m_gridMaxX; ++x) {
		for (LegoS32 z = p_link->m_gridMinZ; z < p_link->m_gridMaxZ; ++z) {
			GetCell(x, z)->Insert(p_link, m_allocator);
		}
	}
}

// STUB: LEGORACERS2 0x00508840
void StaticSpatialGrid::Remove(SpatialIndexLink* p_link)
{
	for (LegoS32 x = p_link->m_gridMinX; x < p_link->m_gridMaxX; ++x) {
		for (LegoS32 z = p_link->m_gridMinZ; z < p_link->m_gridMaxZ; ++z) {
			GetCell(x, z)->Remove(p_link, m_allocator);
		}
	}
}

// FUNCTION: LEGORACERS2 0x00508890
void StaticSpatialGrid::Update(SpatialIndexLink* p_link)
{
	Remove(p_link);
	Insert(p_link);
}
