#ifndef LR2_MODELSKIN_H
#define LR2_MODELSKIN_H

#include "math/vector3.h"

#include <stddef.h>

class WinFile;
struct ModelResource;

struct ModelSkinGroup {
	LegoS32 m_vertexCount;
	void* m_lookupData;
	Vector3* m_positions;
};
DECOMP_SIZE_ASSERT(ModelSkinGroup, 0x0c);

struct ModelSkin {
	LegoU32 m_unk0x00;
	ModelResource* m_model;
	LegoU32 m_unk0x08;
	LegoU32 m_unk0x0c;
	LegoS32 m_groupCount;
	undefined m_unk0x14[4];
	void* m_blendVertices;
	ModelSkinGroup* m_groups;

	ModelSkin(ModelResource* p_model, WinFile* p_file);
	void ReleaseStorage();
};
DECOMP_SIZE_ASSERT(ModelSkin, 0x20);
DECOMP_STATIC_ASSERT(offsetof(ModelSkin, m_groupCount) == 0x10);
DECOMP_STATIC_ASSERT(offsetof(ModelSkin, m_blendVertices) == 0x18);
DECOMP_STATIC_ASSERT(offsetof(ModelSkin, m_groups) == 0x1c);

#endif
