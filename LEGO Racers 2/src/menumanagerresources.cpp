#include "menumanagerresources.h"

#include "gamecontroller.h"
#include "globaloverlaymanager.h"
#include "menupanelset.h"
#include "secondarymanager.h"

void __fastcall LoadControllerPauseMenuTextures(void*, void*, LegoU32);
void __fastcall ReleaseControllerPauseMenuTextures(void*, void*, LegoU32);

// STUB: LEGORACERS2 0x0041c500
void __fastcall LoadMenuManagerResources(MenuManager* p_manager, void*, LegoU32 p_mode)
{
	LoadControllerPauseMenuTextures(g_gameController.m_unk0x90, NULL, p_mode);
	CreateMenuManagerWidgets(p_manager, NULL, p_mode);
	SecondaryManager* secondary = (SecondaryManager*) g_gameController.m_secondaryManager;
	((MenuPanelSet*) secondary->m_unk0x84)->Begin((p_mode != 0) + 1);

	LegoU32 parameters[21];
	parameters[0] = parameters[1] = parameters[2] = parameters[3] = 0;
	parameters[4] = parameters[5] = 0;
	parameters[6] = parameters[7] = 0x3f800000;
	// Retail leaves tile width/height (8,9) untouched for this untiled sprite.
	parameters[10] = parameters[11] = 0xffffffff;
	parameters[12] = 0;
	parameters[13] = parameters[14] = 0x3f800000;
	parameters[15] = parameters[16] = parameters[17] = 0;
	parameters[18] = 1;
	parameters[19] = 0;
	parameters[20] = 8;
	p_manager->m_transitionCover = g_globalOverlayManager.AllocateSprite(parameters);
	p_manager->m_transitionCover->SetVisible(0);
}

// STUB: LEGORACERS2 0x0041c680
void __fastcall ReleaseMenuManagerResources(MenuManager* p_manager, void*, LegoU32 p_mode)
{
	if (p_manager->m_transitionCover != NULL) {
		p_manager->m_transitionCover->Release();
		p_manager->m_transitionCover = NULL;
	}
	SecondaryManager* secondary = (SecondaryManager*) g_gameController.m_secondaryManager;
	((MenuPanelSet*) secondary->m_unk0x84)->End();
	DestroyMenuManagerWidgets(p_manager, NULL, p_mode);
	ReleaseControllerPauseMenuTextures(g_gameController.m_unk0x90, NULL, p_mode);
}

// STUB: LEGORACERS2 0x0041c710
void __fastcall CreateMenuManagerWidgets(MenuManager* p_manager, void*, LegoU32 p_mode)
{
	CreateCommonMenuWidgets(p_manager, NULL);
	if (p_mode == 0) {
		CreateFrontendMenuWidgets(p_manager, NULL);
	}
}

// STUB: LEGORACERS2 0x0041c730
void __fastcall DestroyMenuManagerWidgets(MenuManager* p_manager, void*, LegoU32 p_mode)
{
	if (p_mode == 0) {
		DestroyFrontendMenuWidgets(p_manager, NULL);
	}
	DestroyCommonMenuWidgets(p_manager, NULL);
}
