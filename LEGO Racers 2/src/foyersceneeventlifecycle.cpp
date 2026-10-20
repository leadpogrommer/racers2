#include "bodypartmanager.h"
#include "foyerassetcache.h"
#include "foyersceneevents.h"
#include "foyersceneeventstate.h"
#include "gamecontroller.h"
#include "memoryheap.h"

#include <new>

struct FoyerSceneEventStateTemplate : GameObjectTemplate {
	CharBuffer24 m_firstDefaultName;
	CharBuffer24 m_secondDefaultName;
	LegoU32 m_runtimeValues[4];
};

DECOMP_SIZE_ASSERT(FoyerSceneEventStateTemplate, 0x74);

struct FoyerSceneEventResourcePrefix {
	GameObject* m_loadedAsset;
	LegoU32 m_assetType;
	void* m_runtimeAsset;
	LegoU32 m_unk0x0c;
	LegoU32 m_unk0x10;
	LegoU32 m_unk0x14;
	LegoU32 m_unk0x18;
	LegoU32 m_unk0x1c;
	LegoU32 m_unk0x20;
	LegoU32 m_hasAttachedAsset;
	LegoU8 m_opaque0x28[0x54];
	FoyerSceneEventStateObject* m_stateObject;
};

DECOMP_STATIC_ASSERT(offsetof(FoyerSceneEventResourcePrefix, m_runtimeAsset) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneEventResourcePrefix, m_hasAttachedAsset) == 0x24);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneEventResourcePrefix, m_stateObject) == 0x7c);

// STUB: LEGORACERS2 0x0047c090
void __fastcall InitializeFoyerSceneEventState(void* p_eventStorage, void*)
{
	FoyerSceneEventResourcePrefix* event = (FoyerSceneEventResourcePrefix*) p_eventStorage;
	FoyerSceneEventStateTemplate stateTemplate;
	void* allocation;
	FoyerSceneEventStateObject* state;
	LegoU32 i;

	stateTemplate.m_flags = 0x0a;
	stateTemplate.m_first = CharBuffer24("NoName");
	stateTemplate.m_second = CharBuffer24("");
	stateTemplate.m_firstDefaultName = CharBuffer24("");
	stateTemplate.m_secondDefaultName = CharBuffer24("");
	for (i = 0; i < 4; ++i) {
		stateTemplate.m_runtimeValues[i] = 0;
	}

	allocation = AllocateHeapMemory(sizeof(FoyerSceneEventStateObject));
	state = allocation != NULL ? new (allocation) FoyerSceneEventStateObject(&stateTemplate) : NULL;
	event->m_stateObject = state;
	state->m_vtable->m_initialize(state, NULL);
	state->m_flags |= 0x00400000;
	state->m_stateFlags &= ~2;
	if ((state->m_flags & 4) == 0) {
		state->m_flags |= 4;
	}

	if (event->m_assetType != 0) {
		GetFoyerAssetCache();
		event->m_loadedAsset = (GameObject*) AcquireFoyerAsset(event->m_assetType);
	}
	if (event->m_hasAttachedAsset != 0) {
		LegoU32* attached = (LegoU32*) ((LegoU8*) event->m_loadedAsset + 0x39c);
		LegoU32 faceTextureGroup = *(LegoU32*) ((LegoU8*) attached + 0x84);
		event->m_runtimeAsset = attached;
		for (i = 0; i < 8; ++i) {
			SetBodypartFaceTextureLoaded(
				(BodypartManager*) g_gameController.m_bodypartManager,
				NULL,
				faceTextureGroup,
				i,
				1
			);
		}
	}
}

// STUB: LEGORACERS2 0x0047c1f0
void __fastcall ReleaseFoyerSceneEventState(void* p_eventStorage, void*)
{
	FoyerSceneEventResourcePrefix* event = (FoyerSceneEventResourcePrefix*) p_eventStorage;
	FoyerSceneEventStateObject* state = event->m_stateObject;

	if (event->m_hasAttachedAsset != 0 && g_gameController.m_bodypartManager != NULL) {
		LegoU32 faceTextureGroup = *(LegoU32*) ((LegoU8*) event->m_runtimeAsset + 0x84);
		LegoS32 i;
		for (i = 7; i >= 0; --i) {
			SetBodypartFaceTextureLoaded(
				(BodypartManager*) g_gameController.m_bodypartManager,
				NULL,
				faceTextureGroup,
				i,
				0
			);
		}
	}
	if (event->m_assetType != 0) {
		FoyerAssetCache* cache = GetFoyerAssetCache();
		ReleaseFoyerAsset(cache, NULL, event->m_assetType);
	}
	state->m_vtable->m_remove(state, NULL);
	if (state != NULL) {
		state->m_vtable->m_destroy(state, NULL, 1);
	}
	event->m_stateObject = NULL;
}
