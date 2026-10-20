#include "game/character/bodypartresources.h"

#include "memory/memoryheap.h"
#include "render/core/renderer.h"
#include "render/models/modelresourceloading.h"

struct CharacterHeadResourceEntry {
	CharBuffer128 m_path;
	LegoU32 m_flags;
	ModelResource* m_model;
};
struct CharacterTextureResourceEntry {
	CharBuffer128 m_path;
	LegoS32 m_texture;
};
DECOMP_SIZE_ASSERT(CharacterHeadResourceEntry, 0x88);
DECOMP_SIZE_ASSERT(CharacterTextureResourceEntry, 0x84);

// STUB: LEGORACERS2 0x0044d870
const CharBuffer128* BodypartManager::GetHeadModelPath(LegoS32 p_index, LegoU32 p_species)
{
	CharacterHeadResourceEntry* entries;
	if (p_species == 1) {
		entries = (CharacterHeadResourceEntry*) m_alienHeadModels;
	}
	else if (p_species == 2) {
		entries = (CharacterHeadResourceEntry*) m_ramaHeadModels;
	}
	else {
		entries = (CharacterHeadResourceEntry*) m_headModels;
	}
	return &entries[p_index].m_path;
}

// STUB: LEGORACERS2 0x0044d970
void __fastcall LoadCharacterVisualHeadModels(BodypartManager* p_manager, void*)
{
	for (LegoS32 i = 0; i < p_manager->m_headModelCount; ++i) {
		CharacterHeadResourceEntry* entry = (CharacterHeadResourceEntry*) p_manager->m_headModels + i;
		if ((entry->m_flags & 1) != 0) {
			ModelResource* model = LoadModelResource(entry->m_path.m_data, 0, 1, 0);
			entry = (CharacterHeadResourceEntry*) p_manager->m_headModels + i;
			entry->m_model = model;
			entry->m_flags |= 2;
		}
	}
}

// STUB: LEGORACERS2 0x0044d9e0
void __fastcall ReleaseCharacterVisualHeadModels(BodypartManager* p_manager, void*)
{
	for (LegoS32 i = p_manager->m_headModelCount - 1; i >= 0; --i) {
		CharacterHeadResourceEntry* entry = (CharacterHeadResourceEntry*) p_manager->m_headModels + i;
		if ((entry->m_flags & 2) != 0) {
			if (entry->m_model != NULL) {
				ModelResource* model = entry->m_model;
				model->~ModelResource();
				FreeHeapMemory(model);
			}
			entry = (CharacterHeadResourceEntry*) p_manager->m_headModels + i;
			entry->m_model = NULL;
			entry->m_flags &= ~2;
		}
	}
}

// STUB: LEGORACERS2 0x0044dfe0
void __fastcall SetBodypartLegTextureLoaded(BodypartManager* p_manager, void*, LegoS32 p_index, LegoU32 p_load)
{
	CharacterTextureResourceEntry* entries = (CharacterTextureResourceEntry*) p_manager->m_legTextures;
	if (p_load != 0) {
		LegoS32 texture = g_displayRenderer->CreateTextureResource(entries[p_index].m_path.m_data, 0, -1, 1, 1);
		((CharacterTextureResourceEntry*) p_manager->m_legTextures)[p_index].m_texture = texture;
	}
	else {
		g_displayRenderer->ReleaseTextureResourceReference(entries[p_index].m_texture, 1);
	}
}

// STUB: LEGORACERS2 0x0044e060
void __fastcall SetBodypartTorsoTextureLoaded(
	BodypartManager* p_manager,
	void*,
	LegoS32 p_index,
	LegoU32 p_species,
	LegoU32 p_load
)
{
	void** storage = &p_manager->m_torsoTextures;
	if (p_species == 1) {
		storage = &p_manager->m_alienTorsoTextures;
	}
	else if (p_species == 2) {
		storage = &p_manager->m_ramaTorsoTextures;
	}
	CharacterTextureResourceEntry* entries = (CharacterTextureResourceEntry*) *storage;
	if (p_load != 0) {
		LegoS32 texture = g_displayRenderer->CreateTextureResource(entries[p_index].m_path.m_data, 0, -1, 1, 1);
		((CharacterTextureResourceEntry*) *storage)[p_index].m_texture = texture;
	}
	else {
		g_displayRenderer->ReleaseTextureResourceReference(entries[p_index].m_texture, 1);
	}
}

// STUB: LEGORACERS2 0x0044e210
void __fastcall InitializeSecondaryCharacterResources(void* p_object, void*)
{
	BodypartManager* manager = (BodypartManager*) p_object;
	LoadCharacterVisualHeadModels(manager, NULL);
	LegoS32 count = manager->m_torsoTextureCount;
	LegoS32 i;
	for (i = count - 9; i < count; ++i) {
		SetBodypartTorsoTextureLoaded(manager, NULL, i, 0, 1);
	}
	count = manager->m_legTextureCount;
	for (i = count - 9; i < count; ++i) {
		SetBodypartLegTextureLoaded(manager, NULL, i, 1);
	}
	count = manager->m_faceTextureCount;
	for (i = count - 9; i < count; ++i) {
		SetBodypartFaceTextureLoaded(manager, NULL, i, 0, 1);
		SetBodypartFaceTextureLoaded(manager, NULL, i, 1, 1);
		SetBodypartFaceTextureLoaded(manager, NULL, i, 6, 1);
	}
}

// STUB: LEGORACERS2 0x0044e290
void __fastcall ReleaseSecondaryCharacterResources(void* p_object, void*)
{
	BodypartManager* manager = (BodypartManager*) p_object;
	LegoS32 count = manager->m_faceTextureCount;
	LegoS32 i;
	for (i = count - 1; i >= count - 9; --i) {
		SetBodypartFaceTextureLoaded(manager, NULL, i, 6, 0);
		SetBodypartFaceTextureLoaded(manager, NULL, i, 1, 0);
		SetBodypartFaceTextureLoaded(manager, NULL, i, 0, 0);
	}
	count = manager->m_legTextureCount;
	for (i = count - 1; i >= count - 9; --i) {
		SetBodypartLegTextureLoaded(manager, NULL, i, 0);
	}
	count = manager->m_torsoTextureCount;
	for (i = count - 1; i >= count - 9; --i) {
		SetBodypartTorsoTextureLoaded(manager, NULL, i, 0, 0);
	}
	ReleaseCharacterVisualHeadModels(manager, NULL);
}
