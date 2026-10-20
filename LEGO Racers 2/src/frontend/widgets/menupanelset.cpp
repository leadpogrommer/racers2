#include "frontend/widgets/menupanelset.h"

#include "frontend/widgets/menupanel.h"
#include "game/controller/gamecontroller.h"
#include "memory/memoryheap.h"

#include <new.h>

void __fastcall LoadControllerBuilderTextures(void*, void*, LegoU32);
void __fastcall ReleaseControllerBuilderTextures(void*, void*, LegoU32);

// STUB: LEGORACERS2 0x00419410
void MenuPanelSet::Initialize()
{
	m_unk0x20 = m_unk0x24 = -1;
	m_resourceMode = 0;
	for (LegoU32 i = 0; i < 6; ++i) {
		if (m_panels[i] == NULL) {
			void* storage = AllocateHeapMemory(sizeof(MenuPanel));
			m_panels[i] = storage == NULL ? NULL : new (storage) MenuPanel;
			m_panels[i]->Initialize();
		}
	}
	m_panels[1]->m_listId = 10;
}

// STUB: LEGORACERS2 0x00419470
void MenuPanelSet::Release()
{
	if (m_resourceMode != 0) {
		ReleaseControllerBuilderTextures(g_gameController.m_unk0x90, NULL, m_resourceMode);
	}
	for (LegoS32 i = 5; i >= 0; --i) {
		if (m_panels[i] != NULL) {
			m_panels[i]->Release();
			if (m_panels[i] != NULL) {
				FreeHeapMemory(m_panels[i]);
			}
			m_panels[i] = NULL;
		}
	}
}

// STUB: LEGORACERS2 0x004194c0
void MenuPanelSet::Begin(LegoU32 p_mode)
{
	if (m_resourceMode == 0 && p_mode != 0) {
		m_resourceMode = p_mode;
		LoadControllerBuilderTextures(g_gameController.m_unk0x90, NULL, p_mode);
		SetActive(1);
	}
}

// STUB: LEGORACERS2 0x004194f0
void MenuPanelSet::End()
{
	if (m_resourceMode != 0) {
		ReleaseControllerBuilderTextures(g_gameController.m_unk0x90, NULL, m_resourceMode);
		SetActive(0);
		m_resourceMode = 0;
	}
}

// STUB: LEGORACERS2 0x00419680
void MenuPanelSet::SetActive(LegoU32 p_active)
{
	if (m_resourceMode != 0 || p_active != 0) {
		m_active = p_active;
		if (p_active != 0) {
			m_panels[0]->Activate(0, g_gameController.m_menuManager);
			return;
		}
		for (LegoU32 i = 0; i < 6; ++i) {
			m_panels[i]->Deactivate();
		}
	}
}
