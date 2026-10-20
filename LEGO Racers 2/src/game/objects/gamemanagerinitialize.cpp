#include "core/application/application.h"
#include "core/debug/diagnostics.h"
#include "game/objects/gamemanager.h"
#include "memory/memoryheap.h"

// GLOBAL: LEGORACERS2 0x00549bb0
LegoU32 g_gameObjectInitializationValue = 100;

void ResetGameObjectInitializationValue();
static void InitializeSecondaryStaticSubsystems();

// Original 0x0049a490. Its constructed static dispatch targets have no
// observable effects; see docs/work-game-manager-static-passes.md.
static void InitializeRegisteredStaticSubsystems()
{
	// The 31 indirect slot-8 calls in retail resolve to 0x00403910, a bare
	// `xor eax,eax; ret`; the leading 0x00479030 call is a bare `ret`.
	// Their results are discarded, so this pass has no observable effects.
}

// STUB: LEGORACERS2 0x004a3170
void GameManager::ResizeObjectRegistry(LegoS32 p_capacity)
{
	if (g_gameObjectRegistryUsesStaticStorage == 0) {
		if (p_capacity == 0) {
			return;
		}
	}
	else if (p_capacity == 0) {
		Application* application = GetApplication();
		p_capacity = (LegoS32) application->m_vtable->m_value0x64(application, NULL);
	}

	LegoU32 allocationSize = (LegoU32) p_capacity * sizeof(GameObject*);
	MemoryHeap* heap = GetMemoryHeap();
	GameObject** newObjects = (GameObject**) heap->Allocate(allocationSize, "Main Object List");

	for (LegoS32 i = 0; i < g_gameObjectCount; i++) {
		newObjects[i] = g_gameObjects[i];
	}

	if (g_gameObjectRegistryUsesStaticStorage == 0) {
		heap->Free(g_gameObjects);
	}

	g_gameObjectRegistryUsesStaticStorage = 0;
	g_gameObjects = newObjects;
	g_gameObjectCapacity = p_capacity;

	if (g_gameObjectsByType != NULL) {
		heap->Free(g_gameObjectsByType);
	}
	if (g_gameObjectsWithActiveComponents != NULL) {
		heap->Free(g_gameObjectsWithActiveComponents);
	}

	g_gameObjectsByType = (GameObject**) heap->Allocate(allocationSize, "Main physical List");
	g_gameObjectsWithActiveComponents = (GameObject**) heap->Allocate(allocationSize, "Main collidable List");
	g_gameObjectWorkListsValid = 0;
}

// STUB: LEGORACERS2 0x004a33d0
void GameManager::SortObjectsByParent()
{
	if (g_gameObjectCount <= 0) {
		g_gameObjectWorkListsValid = 0;
		return;
	}

	for (LegoS32 childIndex = 0; childIndex < g_gameObjectCount; childIndex++) {
		GameObject* child = g_gameObjects[childIndex];
		GameObject* parent = *(GameObject**) ((LegoU8*) child + 0x40);
		if (parent == NULL || parent->m_type > child->m_type) {
			continue;
		}

		LegoS32 parentIndex = 0;
		for (; parentIndex < childIndex; parentIndex++) {
			if (g_gameObjects[parentIndex] == parent) {
				break;
			}
		}
		if (parentIndex < childIndex) {
			continue;
		}

		for (parentIndex = childIndex + 1; parentIndex < g_gameObjectCount; parentIndex++) {
			if (g_gameObjects[parentIndex] == parent) {
				break;
			}
		}
		if (parentIndex == g_gameObjectCount) {
			ReportFatalError("failed to find the parent object in the list!");
		}

		--childIndex;
		g_gameObjects[childIndex + 1] = parent;
		g_gameObjects[parentIndex] = child;
	}

	g_gameObjectWorkListsValid = 0;
}

// STUB: LEGORACERS2 0x004a3580
void GameManager::InitializeRegisteredObjects()
{
	ResizeObjectRegistry(0);
	SortObjectsByParent();
	ResetGameObjectInitializationValue();

	for (LegoS32 i = 0; i < g_gameObjectCount; i++) {
		GameObject* object = g_gameObjects[i];
		if ((object->m_flags & 1) != 0) {
			Application* application = GetApplication();
			typedef void(__cdecl * ApplicationInitializeCallback)(Application*, void*);
			ApplicationInitializeCallback callback =
				(ApplicationInitializeCallback) application->m_vtable->m_action0x10;
			callback(application, (LegoU8*) object + 0x10);

			typedef void(__fastcall * ObjectInitializeCallback)(GameObject*, void*);
			ObjectInitializeCallback objectCallback =
				(ObjectInitializeCallback) * (void**) ((LegoU8*) object->m_vtable + 0x10);
			objectCallback(object, NULL);
		}
	}

	m_unk0x00 = 1;
	InitializeSecondaryStaticSubsystems();
	InitializeRegisteredStaticSubsystems();
}

// STUB: LEGORACERS2 0x004a6190
void ResetGameObjectInitializationValue()
{
	if (!GetApplication()->IsDevelopmentMode()) {
		g_gameObjectInitializationValue = 100;
	}
}

// Original 0x004e7080. Its constructed static dispatch targets have no
// observable effects; see docs/work-game-manager-static-passes.md.
static void InitializeSecondaryStaticSubsystems()
{
	// Retail walks 25 pointer globals and calls each object's slot +8. The
	// complete PE vtable inventory is: 21 targets at 0x00403910 (return 0),
	// two at 0x0049a730 (return 2), one at 0x00403920 (return 1), and one at
	// 0x004ba110 (return 3). Every result is discarded; none of those callees
	// writes memory. The leading 0x00479030 target is also a bare `ret`.
}
