#include "gamemanager.h"

#include "renderer.h"
#include "rendereranimationresource.h"
#include "rendererrecords.h"

#include <stdlib.h>

// GLOBAL: LEGORACERS2 0x00578b68
static GameObject* g_initialGameObjects[32];

// GLOBAL: LEGORACERS2 0x005499ac
LegoU32 g_gameObjectRegistryUsesStaticStorage = 1;

// GLOBAL: LEGORACERS2 0x005499b0
GameObject** g_gameObjects = g_initialGameObjects;

// GLOBAL: LEGORACERS2 0x005499b4
LegoS32 g_gameObjectCapacity = 32;

// GLOBAL: LEGORACERS2 0x005499b8
LegoU32 g_gameLifecycleHooksNeedRefresh = 1;

// GLOBAL: LEGORACERS2 0x00578bf8
LegoS32 g_gameObjectCount = 0;

// GLOBAL: LEGORACERS2 0x00578bfc
LegoS32 g_gameObjectsWithActiveComponentsCount = 0;

// GLOBAL: LEGORACERS2 0x00578c00
LegoS32 g_gameObjectsByTypeCount = 0;

// GLOBAL: LEGORACERS2 0x00578c04
GameObject** g_gameObjectsByType = NULL;

// GLOBAL: LEGORACERS2 0x00578c08
GameObject** g_gameObjectsWithActiveComponents = NULL;

// GLOBAL: LEGORACERS2 0x00578c0c
LegoU32 g_gameObjectWorkListsValid = 0;

// GLOBAL: LEGORACERS2 0x00578be8
GameManager g_gameManagerStorage = {0, 0};

// GLOBAL: LEGORACERS2 0x00578bf4
LegoU8 g_gameManagerInitializationFlag = 0;

static void GameManagerStaticDestructor();

// STUB: LEGORACERS2 0x004a2dd0
GameManager* GetGameManager()
{
	if ((g_gameManagerInitializationFlag & 1) == 0) {
		g_gameManagerInitializationFlag |= 1;
		g_gameManagerStorage.m_unk0x00 = 0;
		g_gameManagerStorage.m_unk0x04 = 0;
		atexit(GameManagerStaticDestructor);
	}
	return &g_gameManagerStorage;
}

// Retail no-op static destructor callback at 0x004a2e10; deliberately left
// unannotated so it can fold with the canonical empty-return helper.
static void GameManagerStaticDestructor()
{
}

// STUB: LEGORACERS2 0x004a34a0
void GameManager::RebuildObjectWorkLists()
{
	g_gameObjectsByTypeCount = 0;
	g_gameObjectsWithActiveComponentsCount = 0;

	for (LegoS32 i = 0; i < g_gameObjectCount; i++) {
		GameObject* object = g_gameObjects[i];
		LegoBool32 componentIsActive = FALSE;
		GameLifecycleComponent* component = object->m_lifecycle0x54;

		if (component != NULL) {
			const GameLifecycleDispatchView* dispatch = component->m_vtable;
			componentIsActive = dispatch->m_isActive(component, NULL, 0);
			if (component->m_collisionProxy != NULL && componentIsActive) {
				g_gameObjectsWithActiveComponents[g_gameObjectsWithActiveComponentsCount++] = object;
			}
		}

		LegoS32 type = ((LegoS32) object->m_type) >> 8;
		if (componentIsActive || type == 3 || type == 4 || type == 5 || type == 6 || type == 14) {
			g_gameObjectsByType[g_gameObjectsByTypeCount++] = object;
		}
	}

	g_gameObjectWorkListsValid = 1;
}

// STUB: LEGORACERS2 0x004a36c0
void GameManager::UpdateObjects(LegoU32 p_frameParameter)
{
	GameObject** current = g_gameObjects;
	LegoU32 remaining = (LegoU32) g_gameObjectCount;
	while (remaining != 0) {
		GameObject* object = *current++;
		LegoU32 flags = object->m_flags;
		if (((flags >> 2) & 4) == 0 && (flags & 4) == 0) {
			object->m_vtable->m_update(object, NULL, p_frameParameter);
		}
		--remaining;
	}

	((DisplayRenderer*) g_displayRenderer)->UpdateAnimatedResources();
}

// STUB: LEGORACERS2 0x004a3710
void GameManager::RemoveAllObjects()
{
	LegoU32 index = 0;
	LegoU32 observedCount = (LegoU32) g_gameObjectCount;
	while (index < (LegoU32) g_gameObjectCount) {
		GameObject* object = g_gameObjects[index];
		LegoU32 currentCount = observedCount;
		if ((object->m_flags & 0x00400000) == 0) {
			object->m_vtable->m_remove(object, NULL);
			currentCount = (LegoU32) g_gameObjectCount;
		}

		if (observedCount != currentCount) {
			LegoS32 difference = (LegoS32) observedCount - (LegoS32) currentCount;
			if (difference >= 0) {
				LegoS32 backwards = 0;
				while (backwards <= difference) {
					if (index >= (LegoU32) backwards) {
						LegoU32 candidateIndex = index - (LegoU32) backwards;
						if (candidateIndex < currentCount && g_gameObjects[candidateIndex] == object) {
							index = candidateIndex;
							break;
						}
					}
					++backwards;
				}
			}
		}

		++index;
		observedCount = currentCount;
	}
}

// FUNCTION: LEGORACERS2 0x004a3ca0
void ShutdownGameManager()
{
	GetGameManager()->RemoveAllObjects();
}

// STUB: LEGORACERS2 0x004c0910
void DisplayRenderer::UpdateAnimatedResources()
{
	for (LegoS32 i = 0; i < m_resourceCount0x9c; i++) {
		RendererResourceRef* reference = m_resources0x98[i];
		RendererAnimationResourceRecord* animation = (RendererAnimationResourceRecord*) reference->m_resource;
		RendererAnimationResourceDispatch* dispatch = (RendererAnimationResourceDispatch*) animation->m_vtable;
		dispatch->m_noOp(animation, NULL);
	}

	typedef void(__cdecl * ResourceModeCallback)(RendererAuxiliaryResourceRecord*, LegoU32);
	for (LegoS32 j = 0; j < m_resourceCount0xa4; j++) {
		RendererAuxiliaryResourceRecord* resource = (RendererAuxiliaryResourceRecord*) m_resources0xa0[j];
		ResourceModeCallback callback = (ResourceModeCallback) resource->m_callback;
		if (callback != NULL) {
			callback(resource, 3);
		}
	}

	for (LegoS32 k = 0; k < m_resourceCount0xa4; k++) {
		RendererAuxiliaryResourceRecord* resource = (RendererAuxiliaryResourceRecord*) m_resources0xa0[k];
		ResourceModeCallback callback = (ResourceModeCallback) resource->m_callback;
		if (callback != NULL) {
			callback(resource, 1);
		}
	}
}
