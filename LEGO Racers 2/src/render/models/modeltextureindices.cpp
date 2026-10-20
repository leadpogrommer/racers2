#include "render/models/modelresource.h"

// STUB: LEGORACERS2 0x004a95b0
void ModelResource::RemapTextureIndices(LegoS32 p_oldIndex, LegoS16 p_newIndex)
{
	for (LegoS32 group = 0; group < m_groupCount; ++group) {
		ModelTextureGroup* record = &m_groups[group];
		ModelPrimitiveTextureRecord* primitive = record->m_primitives;
		for (LegoS32 index = 0; index < record->m_primitiveCount; ++index, ++primitive) {
			for (LegoS32 slot = (LegoS32) primitive->m_slotCount - 1; slot >= 0; --slot) {
				if (p_oldIndex == -1 || primitive->m_slots[slot].m_texture == p_oldIndex) {
					primitive->m_slots[slot].m_texture = p_newIndex;
				}
			}
		}
	}
}
