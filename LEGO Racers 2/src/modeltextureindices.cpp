#include "modelresource.h"

// STUB: LEGORACERS2 0x004a95b0
void ModelResource::RemapTextureIndices(LegoS32 p_oldIndex, LegoS16 p_newIndex)
{
	for (LegoS32 group = 0; group < m_groupCount; ++group) {
		ModelTextureGroup* record = &m_groups[group];
		ModelPrimitiveTextureRecord* primitive = record->m_primitives;
		for (LegoS32 index = 0; index < record->m_primitiveCount; ++index, ++primitive) {
			for (LegoS32 slot = (LegoS32) primitive->m_slotCount - 1; slot >= 0; --slot) {
				// Eight-byte slot records start at +20. Their maximum count and
				// other fields have not yet been established.
				LegoS16* textureIndex = (LegoS16*) ((LegoU8*) primitive + 0x20 + slot * 8);
				if (p_oldIndex == -1 || *textureIndex == p_oldIndex) {
					*textureIndex = p_newIndex;
				}
			}
		}
	}
}
