#include "bodypartmanager.h"

struct BodypartTextureEntry {
	CharBuffer128 m_path;
	LegoS32 m_textureIndex;
};
struct BodypartHeadEntry {
	CharBuffer128 m_path;
	LegoU32 m_flags;
	ModelResource* m_model;
};
DECOMP_SIZE_ASSERT(BodypartTextureEntry, 0x84);
DECOMP_SIZE_ASSERT(BodypartHeadEntry, 0x88);
DECOMP_STATIC_ASSERT(offsetof(BodypartTextureEntry, m_textureIndex) == 0x80);
DECOMP_STATIC_ASSERT(offsetof(BodypartHeadEntry, m_model) == 0x84);

// FUNCTION: LEGORACERS2 0x0044d8c0
LegoS32 BodypartManager::GetTorsoTexture(LegoS32 p_index, LegoU32 p_species)
{
	if (p_species == 1) {
		return ((BodypartTextureEntry*) m_alienTorsoTextures)[p_index].m_textureIndex;
	}
	if (p_species == 2) {
		return ((BodypartTextureEntry*) m_ramaTorsoTextures)[p_index].m_textureIndex;
	}
	return ((BodypartTextureEntry*) m_torsoTextures)[p_index].m_textureIndex;
}

// FUNCTION: LEGORACERS2 0x0044d920
LegoS32 BodypartManager::GetLegTexture(LegoS32 p_index)
{
	return ((BodypartTextureEntry*) m_legTextures)[p_index].m_textureIndex;
}

// FUNCTION: LEGORACERS2 0x0044d940
LegoS32 BodypartManager::GetFaceTexture(LegoS32 p_group, LegoS32 p_variant)
{
	LegoS32 index = m_faceTextureGroupSize * p_group + p_variant;
	return ((BodypartTextureEntry*) m_faceTextures)[index].m_textureIndex;
}

// FUNCTION: LEGORACERS2 0x0044da70
ModelResource* BodypartManager::GetLoadedHeadModel(LegoS32 p_index)
{
	BodypartHeadEntry* record = &((BodypartHeadEntry*) m_headModels)[p_index];
	if ((record->m_flags & 2) != 0) {
		return record->m_model;
	}
	return NULL;
}
