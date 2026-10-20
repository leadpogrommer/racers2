#include "render/models/modelresource.h"

// STUB: LEGORACERS2 0x004a8e70
ModelPrimitiveTextureRecord::ModelPrimitiveTextureRecord()
{
	m_primitiveCount = 0;
	m_vertexCount = 0;
	m_material = 0;
	m_flags = 0;
	m_heapData = NULL;
	m_vertexBuffer = NULL;
	m_indexBuffer = NULL;
	m_unk0x14 = 0;
	m_slotCount = 0;
	m_coordinateFlags = 0;
	m_coordinateSetCount = 1;
	for (LegoS32 index = 0; index < 4; ++index) {
		m_slots[index].m_unk0x00 = -1;
		m_slots[index].m_texture = -1;
		m_slots[index].m_coordinateSet = 0x0f;
		m_slots[index].m_flags = 3;
	}
}

// FUNCTION: LEGORACERS2 0x004a8ec0
ModelTextureGroup::ModelTextureGroup()
{
	m_primitiveCount = 0;
	m_primitiveVectors = NULL;
	m_primitives = NULL;
}
