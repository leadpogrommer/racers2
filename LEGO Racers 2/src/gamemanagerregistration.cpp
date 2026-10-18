#include "application.h"
#include "gamemanager.h"
#include "memoryheap.h"

extern LegoBool32 g_heapInitializationSucceeded;

// STUB: LEGORACERS2 0x004a3290
void GameManager::RegisterObject(GameObject* p_object)
{
	if (g_gameObjectRegistryUsesStaticStorage != 0) {
		(void) GetMemoryHeap();
		if (g_heapInitializationSucceeded == TRUE) {
			ResizeObjectRegistry(0);
		}
	}

	LegoS32 count = g_gameObjectCount;
	if ((LegoU32) count >= (LegoU32) g_gameObjectCapacity) {
		Application* application = GetApplication();
		if (application->m_vtable->m_value0x68(application, NULL) != 0) {
			ResizeObjectRegistry(g_gameObjectCapacity + 0x100);
		}
		count = g_gameObjectCount;
	}

	LegoS32 insertionIndex = 0;
	if (count > 0) {
		while (insertionIndex < count && g_gameObjects[insertionIndex]->m_type <= p_object->m_type) {
			insertionIndex++;
		}
	}

	if (count > insertionIndex) {
		LegoS32 shiftIndex = count;
		while (shiftIndex > insertionIndex) {
			g_gameObjects[shiftIndex] = g_gameObjects[shiftIndex - 1];
			shiftIndex--;
		}
	}

	g_gameObjects[insertionIndex] = p_object;
	g_gameObjectCount = count + 1;
	g_gameObjectWorkListsValid = 0;
}

// STUB: LEGORACERS2 0x004a3350
void GameManager::UnregisterObject(GameObject* p_object)
{
	LegoU32 count = (LegoU32) g_gameObjectCount;
	LegoU32 index = 0;
	if (count != 0) {
		while (index < count && g_gameObjects[index] != p_object) {
			index++;
		}
	}

	index++;
	if (index >= count) {
		g_gameObjectCount = (LegoS32) (count - 1);
		g_gameObjectWorkListsValid = 0;
		return;
	}

	while (index < count) {
		g_gameObjects[index - 1] = g_gameObjects[index];
		index++;
	}

	g_gameObjectCount = (LegoS32) (count - 1);
	g_gameObjectWorkListsValid = 0;
}
