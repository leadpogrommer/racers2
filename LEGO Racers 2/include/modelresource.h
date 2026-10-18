#ifndef LR2_MODELRESOURCE_H
#define LR2_MODELRESOURCE_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

struct ModelPrimitiveTextureRecord {
	undefined m_prefix[0x18];
	LegoS16 m_slotCount;
	undefined m_remaining[0x3c - 0x1a];
};
struct ModelTextureGroup {
	undefined m_prefix[8];
	LegoS32 m_primitiveCount;
	undefined m_unk0x0c[4];
	ModelPrimitiveTextureRecord* m_primitives;
};

// Observed prefix of the loaded model; the full resource extent is unproven.
struct ModelResource {
	undefined m_prefix[0x0c];
	ModelTextureGroup* m_groups;
	undefined m_unk0x10[0x20 - 0x10];
	LegoS32 m_groupCount;
	void RemapTextureIndices(LegoS32 p_oldIndex, LegoS16 p_newIndex);
};
DECOMP_SIZE_ASSERT(ModelPrimitiveTextureRecord, 0x3c);
DECOMP_SIZE_ASSERT(ModelTextureGroup, 0x14);
DECOMP_STATIC_ASSERT(offsetof(ModelResource, m_groups) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(ModelResource, m_groupCount) == 0x20);

#endif
