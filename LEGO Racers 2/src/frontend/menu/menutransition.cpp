#include "frontend/menu/menumanager.h"
#include "frontend/menu/menumanagercurrentaction.h"
#include "frontend/overlay/globaloverlaymanager.h"
#include "frontend/widgets/menuwidget.h"
#include "game/pause/globalpause.h"

extern LegoU32 g_menuChoiceColorA;
extern LegoU32 g_menuChoiceColorB;
void __fastcall ApplySavedAudioLevels(void*, void*);
void __fastcall ClearMenuManagerDisplay(MenuManager*, void*, LegoU32);
void __fastcall BeginMenuManagerMode(MenuManager*, void*, LegoU32, LegoU32);
void __fastcall PopMenuManagerAction(MenuManager*, void*);
static void __fastcall ClearGlobalPauseEnterRequest(GlobalPauseController*, void*);

// STUB: LEGORACERS2 0x0041dcc0
void __fastcall PopSecondaryMenuActions(MenuManager* p_menu, void*, LegoU32 p_update, LegoS32 p_count)
{
	LegoS32 depth = p_menu->m_unk0x198;
	if (depth < (LegoS32) p_count) {
		return;
	}
	if (p_update != 0) {
		UpdateMenuManagerCurrentAction(p_menu, NULL);
	}
	p_menu->m_unk0x1a8 = 1;
	*(LegoU32*) p_menu->m_unk0x1ac = 0;
	depth = (LegoS32) p_menu->m_unk0x198 - p_count;
	p_menu->m_unk0x198 = depth;
	LegoU8* frame = (LegoU8*) p_menu + depth * 0x14;
	if (depth > 0 && *(LegoU32*) (frame + 0x60) == 0) {
		p_menu->m_unk0x1a0 = *(LegoU32*) (frame + 0x44);
	}
	else {
		p_menu->m_unk0x1a0 = -1;
	}
}

// STUB: LEGORACERS2 0x0041dd30
void __fastcall PopMenuManagerAction(MenuManager* p_menu, void*)
{
	LegoU8* menu = (LegoU8*) p_menu;
	LegoS32* depth = (LegoS32*) (menu + 0x198);
	do {
		if (*depth < 1) {
			return;
		}
		--*depth;
	} while (*(LegoU32*) (menu + 0x60 + *depth * 0x14) == 0);
}

// STUB: LEGORACERS2 0x0041dd80
void __fastcall ResetSecondaryMenuAction(MenuManager* p_menu, void*, LegoU32 p_action)
{
	ClearMenuManagerDisplay(p_menu, NULL, 1);
	PopMenuManagerAction(p_menu, NULL);
	BeginMenuManagerMode(p_menu, NULL, p_action, 0);
}

// STUB: LEGORACERS2 0x0041de70
void __fastcall BeginMenuTransition(GameObject* p_object, void*, LegoU32 p_pop)
{
	MenuManager* menu = (MenuManager*) p_object;
	ClearMenuManagerDisplay(menu, NULL, 1);
	if (p_pop != 0) {
		PopMenuManagerAction(menu, NULL);
	}
	menu->m_unk0x1a8 = 1;
	*(LegoU32*) menu->m_unk0x1ac = 0;
	menu->m_unk0x1a0 = -1;
	ClearGlobalPauseEnterRequest(&g_globalPauseController, NULL);
}

// STUB: LEGORACERS2 0x00420be0
void __fastcall ClearMenuManagerDisplay(MenuManager* p_menu, void*, LegoU32 p_update)
{
	if (p_update != 0) {
		UpdateMenuManagerCurrentAction(p_menu, NULL);
	}
	else if (
		p_menu->m_unk0x19c == 13 || p_menu->m_unk0x19c == 14 || p_menu->m_unk0x19c == 39 || p_menu->m_unk0x19c == 38
	) {
		ApplySavedAudioLevels(NULL, NULL);
	}
	for (LegoU32 i = 0; i < 14; ++i) {
		GlobalOverlayRecord* record = (GlobalOverlayRecord*) p_menu->m_overlayRecords[i];
		LegoU32 white = -1;
		record->SetColor(&white);
		record->m_animationFlags &= ~0x40u;
	}
	for (LegoU32 j = 0; j < 14; ++j) {
		((GlobalOverlayRecord*) p_menu->m_overlayRecords[j])->InitializeTextByIndex(-1, 0, 0.0f);
	}
	if (p_menu->m_transitionCover != NULL) {
		p_menu->m_transitionCover->SetVisible(0);
	}
	for (LegoU32 k = 0; k < 18; ++k) {
		MenuWidgetBase* widget = p_menu->m_widgets[k];
		if (widget != NULL) {
			widget->m_vtable->m_reset(widget, NULL);
		}
	}
}

// FUNCTION: LEGORACERS2 0x004bb970
static void __fastcall ClearGlobalPauseEnterRequest(GlobalPauseController* p_pause, void*)
{
	p_pause->m_enterRequested = 0;
}
