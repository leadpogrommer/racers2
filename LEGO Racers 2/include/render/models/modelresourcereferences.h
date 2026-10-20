#ifndef LR2_MODELRESOURCEREFERENCES_H
#define LR2_MODELRESOURCEREFERENCES_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

// The MDL resource stores texture IDs followed by material IDs in one allocation.
struct ModelResourceReferences {
	LegoS16* m_ids;
	LegoS16 m_textureCount;
	LegoS16 m_materialCount;

	void Allocate(LegoS16 p_textureCount, LegoS16 p_materialCount);
	void FreeStorage();
	void ReleaseRendererReferences();
	void Reset();
	LegoS32 GetTexture(LegoS32 p_index) const;
	LegoS32 GetMaterial(LegoS32 p_index) const;
	void SetTexture(LegoS32 p_index, LegoS16 p_id);
	void SetMaterial(LegoS32 p_index, LegoS16 p_id);
};
DECOMP_SIZE_ASSERT(ModelResourceReferences, 8);
DECOMP_STATIC_ASSERT(offsetof(ModelResourceReferences, m_textureCount) == 4);
DECOMP_STATIC_ASSERT(offsetof(ModelResourceReferences, m_materialCount) == 6);

#endif
