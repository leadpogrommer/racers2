#include "application.h"
#include "gamemanager.h"
#include "inputcontrollermanager.h"

// STUB: LEGORACERS2 0x004e6a00
void GameManager::PrepareComponents()
{
	GameObject** current = g_gameObjectsByType;
	LegoU32 remaining = (LegoU32) g_gameObjectsByTypeCount;
	while (remaining != 0) {
		GameLifecycleComponent* component = (*current)->m_lifecycle0x54;
		if (component != NULL) {
			component->m_vtable->m_prepare(component, NULL);
		}
		++current;
		--remaining;
	}
}

// STUB: LEGORACERS2 0x004e7ae0
void GameManager::SetControllerFrame(const LegoU32* p_frame)
{
	if (!GetApplication()->IsDevelopmentMode()) {
		g_inputControllerManager->m_unk0x18 = *p_frame;
	}
}
