#include "frontend/menu/menubackground.h"
#include "frontend/menu/menumanager.h"
#include "frontend/menu/menumanagercurrentaction.h"
#include "frontend/overlay/globaloverlaymanager.h"
#include "frontend/secondary/secondarymanager.h"
#include "frontend/widgets/menuchoiceselector.h"
#include "frontend/widgets/menupanelset.h"
#include "game/controller/gamecontroller.h"

#include <string.h>

namespace
{
void __fastcall ReleaseMenuActionSprite(GlobalOverlaySprite** p_sprite)
{
	if (*p_sprite != NULL) {
		(*p_sprite)->Release();
		*p_sprite = NULL;
	}
}

void __fastcall ClearMenuActionWidgetResources(
	void* p_widget,
	void*,
	LegoU32 p_releaseFirst,
	LegoU32 p_releaseSecond,
	LegoU32 p_releaseThird
)
{
	LegoU8* widget = (LegoU8*) p_widget;
	if (p_releaseFirst != 0) {
		ReleaseMenuActionSprite((GlobalOverlaySprite**) (widget + 0x34));
		ReleaseMenuActionSprite((GlobalOverlaySprite**) (widget + 0x18));
	}
	GlobalOverlaySprite** cursor = (GlobalOverlaySprite**) (widget + 0x4c);
	for (LegoU32 i = 0; i < 6; ++i, --cursor) {
		if (p_releaseThird != 0) {
			ReleaseMenuActionSprite(cursor + 6);
			ReleaseMenuActionSprite(cursor);
		}
		if (p_releaseSecond != 0) {
			ReleaseMenuActionSprite(cursor - 7);
			ReleaseMenuActionSprite(cursor - 14);
		}
	}
}

void __fastcall SetSecondaryManagerAuxiliaryMode(void* p_auxiliary, void*, LegoU32 p_mode)
{
	LegoU8* auxiliary = (LegoU8*) p_auxiliary;
	*(LegoU32*) (auxiliary + 0x84) = p_mode;
	if (p_mode == 0) {
		ClearMenuActionWidgetResources(auxiliary, NULL, 1, 1, 1);
		*(LegoU32*) (auxiliary + 0xa4) = 0;
		*(LegoU32*) (auxiliary + 0x98) = 0xffffffff;
		*(LegoU32*) (auxiliary + 0x88) = 0;
		*(LegoU32*) (auxiliary + 0x8c) = 0;
	}
}

void __fastcall ResetSecondaryManagerModeState(void* p_state, void*)
{
	*(LegoU32*) ((LegoU8*) p_state + 0xf0) = 0xffffffff;
	*(LegoU32*) ((LegoU8*) p_state + 0xf4) = 0xffffffff;
}

void __fastcall PushMenuManagerAction(MenuManager* p_manager, void*, LegoU32 p_action, LegoU32 p_value, LegoU32 p_state)
{
	LegoU8* manager = (LegoU8*) p_manager;
	LegoS32 depth = *(LegoS32*) (manager + 0x198);
	if (depth > 0) {
		UpdateMenuManagerCurrentAction(p_manager, NULL);
		depth = *(LegoS32*) (manager + 0x198);
	}
	if ((LegoS32) p_value == -1 && depth > 0) {
		p_value = *(LegoU32*) (manager + 0x48 + depth * 0x14);
	}
	LegoU32* record = (LegoU32*) (manager + 0x58 + depth * 0x14);
	record[0] = p_action;
	record[1] = p_value;
	record[2] = p_state;
	*(LegoU32*) (manager + 0x64 + depth * 0x14) = 0;
	LegoU32 selected = *(LegoU32*) (manager + 0x21c);
	if (selected == 0) {
		selected = *(LegoU32*) (manager + 0x218);
	}
	*(LegoU32*) (manager + 0x68 + depth * 0x14) = selected;
	*(LegoU32*) (manager + 0x1a8) = 1;
	*(LegoU32*) (manager + 0x1ac) = 0;
	*(LegoS32*) (manager + 0x198) = depth + 1;
	*(LegoU32*) (manager + 0x1a0) = p_action;
	*(LegoU32*) (manager + 0x21c) = 0;
}

void __fastcall PushMenuManagerMode(MenuManager* p_manager, void*, LegoU32 p_mode, LegoU32 p_selection)
{
	LegoU8* manager = (LegoU8*) p_manager;
	*(LegoS32*) (manager + 0x19c) = -1;
	*(LegoU32*) (manager + 0x1a4) = 0;
	PushMenuManagerAction(p_manager, NULL, p_mode, p_selection, 1);
}

} // namespace

// STUB: LEGORACERS2 0x004126e0
void __fastcall ClearSecondaryManagerAuxiliaryResources(
	void* p_auxiliary,
	void*,
	LegoU32 p_releaseHeader,
	LegoU32 p_releaseOuter,
	LegoU32 p_releaseInner
)
{
	ClearMenuActionWidgetResources(p_auxiliary, NULL, p_releaseHeader, p_releaseOuter, p_releaseInner);
}

// STUB: LEGORACERS2 0x00412f20
void __fastcall SetSecondaryManagerAuxiliaryEnabled(void* p_auxiliary, void*, LegoU32 p_enabled)
{
	SetSecondaryManagerAuxiliaryMode(p_auxiliary, NULL, p_enabled);
}

// STUB: LEGORACERS2 0x00417630
void __fastcall ResetSecondaryManagerModeStateAtEntry(void* p_state, void*)
{
	ResetSecondaryManagerModeState(p_state, NULL);
}

// STUB: LEGORACERS2 0x0041dbf0
void __fastcall PushMenuManagerActionState(
	MenuManager* p_manager,
	void*,
	LegoU32 p_action,
	LegoU32 p_value,
	LegoU32 p_state
)
{
	PushMenuManagerAction(p_manager, NULL, p_action, p_value, p_state);
}

// STUB: LEGORACERS2 0x0041dd50
void __fastcall BeginMenuManagerMode(MenuManager* p_manager, void*, LegoU32 p_action, LegoU32 p_value)
{
	*(LegoS32*) ((LegoU8*) p_manager + 0x19c) = -1;
	*(LegoU32*) ((LegoU8*) p_manager + 0x1a4) = 0;
	PushMenuManagerAction(p_manager, NULL, p_action, p_value, 1);
}

// STUB: LEGORACERS2 0x00423640
void __fastcall BeginMenuManagerChoice(
	MenuManager* p_manager,
	void*,
	const LegoFloat* p_position,
	LegoU32 p_selection,
	LegoU32 p_mode,
	LegoU32 p_left,
	LegoU32 p_right,
	LegoU32 p_widget
)
{
	LegoU8* manager = (LegoU8*) p_manager;
	if (*(LegoU32*) (manager + 0x208) == 0) {
		InitializeMenuChoiceSelectorFromMode(
			p_manager->m_extraChoiceSelector,
			NULL,
			0xffffffff,
			p_position,
			p_selection,
			p_mode,
			p_left,
			p_right,
			p_widget,
			0.8f,
			1,
			1
		);
		*(LegoU32*) (manager + 0x208) = 1;
		*(LegoU32*) (manager + 0x1e4) = 1;
	}
}

// STUB: LEGORACERS2 0x00427d90
void __fastcall EnterSecondaryManagerMode(GameObject* p_object, void*, LegoU32 p_mode)
{
	SecondaryManager* manager = (SecondaryManager*) p_object;
	manager->m_state = 2;
	BeginMenuManagerMode((MenuManager*) g_gameController.m_menuManager, NULL, p_mode, 0);
	((MenuBackground*) manager->m_unk0x80)->SetMode(2);
	((MenuPanelSet*) manager->m_unk0x84)->SetActive(1);
	SetSecondaryManagerAuxiliaryEnabled(manager->m_unk0x88, NULL, 1);
	ResetSecondaryManagerModeStateAtEntry(manager->m_unk0x17c, NULL);
}

// STUB: LEGORACERS2 0x00427e40
void __fastcall SelectSecondaryManagerChoice(
	GameObject* p_object,
	void*,
	const LegoFloat* p_position,
	LegoU32 p_selection,
	LegoU32 p_mode,
	LegoU32 p_left,
	LegoU32 p_right,
	LegoU32 p_widget
)
{
	SecondaryManager* manager = (SecondaryManager*) p_object;
	manager->m_state = 2;
	BeginMenuManagerChoice(
		(MenuManager*) g_gameController.m_menuManager,
		NULL,
		p_position,
		p_selection,
		p_mode,
		p_left,
		p_right,
		p_widget
	);
	((MenuPanelSet*) manager->m_unk0x84)->SetActive(1);
}
