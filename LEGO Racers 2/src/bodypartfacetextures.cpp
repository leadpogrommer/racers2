#include "bodypartmanager.h"
#include "renderer.h"

struct BodypartFaceTextureRecord {
	LegoChar m_path[0x80];
	LegoS32 m_textureIndex;
};

DECOMP_SIZE_ASSERT(BodypartFaceTextureRecord, 0x84);

// STUB: LEGORACERS2 0x0044e190
void __fastcall SetBodypartFaceTextureLoaded(
	BodypartManager* p_manager,
	void*,
	LegoU32 p_group,
	LegoU32 p_variant,
	LegoU32 p_load
)
{
	LegoU32 index = p_manager->m_faceTextureGroupSize * p_group + p_variant;
	BodypartFaceTextureRecord* record = (BodypartFaceTextureRecord*) p_manager->m_faceTextures + index;
	if (p_load != 0) {
		record->m_textureIndex = g_displayRenderer->CreateTextureResource(record->m_path, 0, -1, 1, 1);
	}
	else {
		g_displayRenderer->ReleaseTextureResourceReference(record->m_textureIndex, 1);
	}
}
