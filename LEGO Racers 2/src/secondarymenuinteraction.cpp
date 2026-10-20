#include "secondarymenuinteraction.h"

#include "audioservice.h"
#include "gamecontroller.h"
#include "gamecontrollersession.h"
#include "gamecontrollersessionqueries.h"
#include "globaloverlaymanager.h"
#include "menuchoiceselector.h"
#include "menulayouttuning.h"
#include "menuwidget.h"

#include <math.h>

void __fastcall BeginMenuTransition(GameObject*, void*, LegoU32);
void __fastcall PopSecondaryMenuActions(MenuManager*, void*, LegoU32, LegoS32);
void __fastcall ClearMenuManagerDisplay(MenuManager*, void*, LegoU32);
void __fastcall BuildMenuManagerAction(MenuManager*, void*, LegoU32, LegoS32);
void __fastcall LayoutMenuOptionWidgets(MenuManager*, void*, LegoU32, LegoU32, LegoS32, LegoFloat*, LegoFloat*);
void __fastcall RefreshMenuNetworkList(MenuManager*, void*);
void __fastcall SetMenuManagerInputEnabled(MenuManager*, void*, LegoU32);
extern LegoU32 g_menuChoiceColorA;
extern LegoU32 g_menuChoiceColorB;
void __fastcall SetMenuManagerWaitForInput(MenuManager*, void*, LegoU32);
void __stdcall PlayMenuInteractionSound(LegoS32);
LegoBool32 __fastcall IsMenuOptionEnabled(MenuManager*, void*, LegoS32);
LegoBool32 __fastcall ProcessMenuWidgetsInput(
	MenuManager*,
	void*,
	const SecondaryManagerMediaSlot*,
	LegoS32*,
	LegoS32*
);
void __fastcall HighlightMenuOption(MenuManager*, void*, LegoS32, LegoU32);
void __fastcall ResetMenuSelectionWidgets(MenuManager*, void*, LegoU32);
LegoBool32 __fastcall ApplyMenuConfirmAction(MenuManager*, void*);
LegoBool32 __fastcall CanMenuCancel(MenuManager*, void*);
void __fastcall UpdateMenuInteractionWidgets(MenuManager*, void*, const SecondaryManagerMediaSlot*);
LegoU32 __fastcall UpdateMenuTransition(MenuManager*, void*, LegoU32);

static LegoS32 GetMenuSelectedOption(MenuManager* p_menu)
{
	if (p_menu->m_unk0x198 == 0) {
		return -1;
	}
	return *(LegoS32*) ((LegoU8*) p_menu + (p_menu->m_unk0x198 * 5 + 0x14) * 4);
}

static void SetMenuSelectedOption(MenuManager* p_menu, LegoS32 p_option)
{
	*(LegoS32*) ((LegoU8*) p_menu + (p_menu->m_unk0x198 + 4) * 0x14) = p_option;
}

// STUB: LEGORACERS2 0x0041caf0
LegoU32 __fastcall UpdateSecondaryMenuManager(
	MenuManager* p_menu,
	void*,
	LegoU32 p_frame,
	LegoU32 p_owner,
	LegoU32 p_manualInputAddress,
	SecondaryManagerMediaState* p_mediaInput
)
{
	LegoU8* menu = (LegoU8*) p_menu;
	LegoS32 depth = p_menu->m_unk0x198;
	if (depth > 0 && p_owner != *(LegoU32*) (menu + 0x48 + depth * 0x14)) {
		return 0;
	}
	p_menu->m_unk0x1a4 = 1;
	LegoBool32 active = TRUE;
	if (depth == 0 && p_menu->m_unk0x208 == 0) {
		active = FALSE;
	}
	if ((p_menu->m_unk0x1e0 & 1) != 0) {
		active = FALSE;
	}
	if (p_menu->m_unk0x1a8 != 0) {
		active = FALSE;
	}
	SecondaryManagerMediaSlot input = {0, 0, 0, 0};
	if (p_mediaInput != NULL) {
		for (LegoU32 i = 0; i < 3; ++i) {
			if (p_mediaInput->IsSlotEnabled(i)) {
				MergeSecondaryMediaSlot(&input, NULL, p_mediaInput->GetSlot(i));
			}
		}
	}
	if (p_manualInputAddress != 0) {
		MergeSecondaryMediaSlot(&input, NULL, (const SecondaryManagerMediaSlot*) p_manualInputAddress);
	}
	MenuNavigationState* navigation =
		(MenuNavigationState*) ((SecondaryManager*) g_gameController.m_secondaryManager)->m_unk0x7c;
	if (navigation != NULL) {
		navigation->ReadDirectionInput(0);
	}
	p_menu->m_unk0x228 = -1;
	if ((p_menu->m_unk0x1e0 & 2) != 0 &&
		(input.m_resourceChangeMask != 0 ||
		 (navigation != NULL && navigation->m_active != 0 && navigation->m_verticalFlags != 0))) {
		SetMenuManagerWaitForInput(p_menu, NULL, 0);
		ReleaseSecondaryObject8c(((SecondaryManager*) g_gameController.m_secondaryManager)->m_unk0x8c, NULL);
		return 0;
	}
	if (*(LegoU32*) (menu + 0x1e4) != 0) {
		if (input.m_resourceChangeMask == 0) {
			*(LegoU32*) (menu + 0x1e4) = 0;
		}
		else {
			active = FALSE;
		}
	}
	if (p_menu->m_unk0x208 != 0 && active) {
		LegoS32 sound = -1;
		LegoU32 result = 0;
		MenuChoiceSelector* choice = p_menu->m_extraChoiceSelector;
		LegoU32 change = choice->m_vtable->m_unk0x10(choice, NULL, (LegoU32*) &sound, (const LegoU32*) &input);
		if ((input.m_resourceChangeMask & 1) != 0 || change == 2) {
			sound = 0;
			p_menu->m_unk0x210 = choice->m_selection;
		}
		else if ((input.m_resourceChangeMask & 0x202) != 0) {
			p_menu->m_unk0x210 = 0;
			sound = 1;
		}
		else {
			if (sound != -1) {
				PlayMenuInteractionSound(sound);
			}
			return result;
		}
		choice->m_vtable->m_reset(choice, NULL);
		LegoU32 action = p_menu->m_unk0x214;
		p_menu->m_unk0x208 = 0;
		p_menu->m_unk0x214 = -1;
		if (action == 0) {
			result = p_menu->m_unk0x210 != 0 ? 5 : 0;
		}
		else if (action == 1) {
			if (p_menu->m_unk0x210 != 0) {
				ApplySecondarySessionData(g_gameController.m_unk0x8c, NULL);
				BeginMenuTransition(p_menu, NULL, 1);
				p_menu->m_unk0x1d8 = 4;
			}
		}
		else if (action == 2) {
			if (p_menu->m_unk0x210 != 0) {
				LegoU8* session = (LegoU8*) GetGameControllerSession();
				*(LegoU32*) (session + 0x1a70) = 0;
				*(LegoU32*) (session + 0x1a74) = 0;
				*(LegoU32*) (session + 0x0c) = *(LegoU32*) (session + 0x1a78);
				*(LegoU32*) (session + 0x10) = *(LegoU32*) (session + 0x1aa0);
				*(LegoU32*) (session + 0x14) = *(LegoU32*) (session + 0x1ac8);
				BeginMenuTransition(p_menu, NULL, 1);
				p_menu->m_unk0x1d8 = 1;
			}
		}
		else {
			result = 14;
		}
		if (sound != -1) {
			PlayMenuInteractionSound(sound);
		}
		return result;
	}
	if (p_menu->m_unk0x22c == 0 && p_menu->m_unk0x1a8 == 0) {
		return 0;
	}
	if (active) {
		LegoS32 previous = GetMenuSelectedOption(p_menu);
		LegoS32 selected = previous;
		LegoS32 movement = 0;
		if ((input.m_stateChangeMask & 8) != 0) {
			movement = 1;
		}
		else if ((input.m_stateChangeMask & 4) != 0) {
			movement = -1;
		}
		LegoBool32 leftClick = FALSE;
		LegoBool32 rightClick = FALSE;
		if (navigation->m_active == 0) {
			navigation = NULL;
		}
		else {
			leftClick = (navigation->m_verticalFlags & 1) != 0;
			rightClick = (navigation->m_verticalFlags & 2) != 0;
			for (LegoS32 option = 0; option < 6; ++option) {
				if (IsMenuOptionEnabled(p_menu, NULL, option) &&
					navigation->IsRecordUnderPointer((GlobalOverlayRecord*) p_menu->m_overlayRecords[option])) {
					p_menu->m_unk0x228 = option;
					break;
				}
			}
			LegoFloat position[2];
			navigation->GetPointerPosition(position);
			if (fabs(*(LegoFloat*) (menu + 0x220) - position[0]) > 0.0001f ||
				fabs(*(LegoFloat*) (menu + 0x224) - position[1]) > 0.0001f) {
				if ((LegoS32) p_menu->m_unk0x228 != -1) {
					movement = p_menu->m_unk0x228 - previous;
				}
				*(LegoFloat*) (menu + 0x220) = position[0];
				*(LegoFloat*) (menu + 0x224) = position[1];
			}
			if (leftClick && (LegoS32) p_menu->m_unk0x228 != -1) {
				movement = p_menu->m_unk0x228 - previous;
			}
		}
		LegoBool32 widgetsHandled = FALSE;
		if (movement != 0) {
			do {
				selected += movement;
				if (selected < 0) {
					selected = 5;
				}
				else if (selected > 5) {
					selected = 0;
				}
			} while (!IsMenuOptionEnabled(p_menu, NULL, selected));
		}
		if (selected != previous) {
			HighlightMenuOption(p_menu, NULL, previous, 0);
			HighlightMenuOption(p_menu, NULL, selected, 1);
			SetMenuSelectedOption(p_menu, selected);
			PlayMenuInteractionSound(3);
			ResetMenuSelectionWidgets(p_menu, NULL, 0);
		}
		else {
			LegoS32 changed = 0;
			LegoS32 sound = -1;
			widgetsHandled = ProcessMenuWidgetsInput(p_menu, NULL, &input, &changed, &sound);
			if (changed != 0 && sound != -1) {
				PlayMenuInteractionSound(sound);
			}
		}
		if (!widgetsHandled) {
			if (((input.m_resourceChangeMask & 1) != 0 || (leftClick && p_menu->m_unk0x228 == (LegoU32) selected)) &&
				ApplyMenuConfirmAction(p_menu, NULL)) {
				PlayMenuInteractionSound(0);
			}
			else if (((input.m_resourceChangeMask & 0x202) != 0 || rightClick) && CanMenuCancel(p_menu, NULL)) {
				PopSecondaryMenuActions(p_menu, NULL, 0, 1);
				PlayMenuInteractionSound(1);
			}
			else if (p_menu->m_unk0x19c == 35 || p_menu->m_unk0x19c == 36) {
				if ((input.m_resourceChangeMask & 0x42) != 0) {
					PopSecondaryMenuActions(p_menu, NULL, 0, 1);
					PlayMenuInteractionSound(1);
					p_menu->m_unk0x1d8 = 7;
				}
			}
			else if (p_menu->m_unk0x19c == 1 && (input.m_resourceChangeMask & 0x42) != 0) {
				HighlightMenuOption(p_menu, NULL, GetMenuSelectedOption(p_menu), 0);
				HighlightMenuOption(p_menu, NULL, 5, 1);
				SetMenuSelectedOption(p_menu, 5);
				if (ApplyMenuConfirmAction(p_menu, NULL)) {
					PlayMenuInteractionSound(0);
				}
			}
		}
	}
	UpdateMenuInteractionWidgets(p_menu, NULL, &input);
	if (p_menu->m_unk0x1a8 != 0) {
		return UpdateMenuTransition(p_menu, NULL, p_frame);
	}
	if (navigation != NULL && (LegoS32) p_menu->m_unk0x228 != -1) {
		navigation->ReadDirectionInput(1);
	}
	return 0;
}

// STUB: LEGORACERS2 0x0041d100
LegoU32 __fastcall UpdateMenuTransition(MenuManager* p_menu, void*, LegoU32 p_frame)
{
	LegoFloat frame = *(LegoFloat*) &p_frame;
	LegoFloat* elapsed = (LegoFloat*) p_menu->m_unk0x1ac;
	*elapsed = frame + *elapsed;
	if (p_menu->m_unk0x1a8 == 1 && *elapsed >= 0.25f) {
		ClearMenuManagerDisplay(p_menu, NULL, 0);
		if ((LegoS32) p_menu->m_unk0x198 > 0) {
			LegoU32 action = p_menu->m_unk0x1a0;
			p_menu->m_unk0x19c = action;
			if ((LegoS32) action != -1) {
				BuildMenuManagerAction(p_menu, NULL, action, GetMenuSelectedOption(p_menu));
			}
		}
		p_menu->m_unk0x1a8 = 2;
		*elapsed -= 0.25f;
	}
	if (p_menu->m_unk0x1a8 == 2 && 0.25f <= *elapsed) {
		LegoU32 action = p_menu->m_unk0x1d8;
		p_menu->m_unk0x1a8 = 0;
		*elapsed = 0.0f;
		p_menu->m_unk0x1d8 = 0;
		return action;
	}
	return 0;
}

// STUB: LEGORACERS2 0x0041d1e0
void __fastcall ResetMenuSelectionWidgets(MenuManager* p_menu, void*, LegoU32 p_measure)
{
	if (p_menu->m_unk0x1dc == 0) {
		return;
	}
	LegoFloat origin[2] = {
		g_menuLayoutMarginX + p_menu->m_panelBottomRight[0],
		g_menuLayoutMarginY + p_menu->m_panelTopLeft[1]
	};
	LegoFloat size[2];
	if (p_measure == 0) {
		size[0] = p_menu->m_secondaryBottomRight[0] - p_menu->m_secondaryTopLeft[0];
		size[1] = p_menu->m_secondaryBottomRight[1] - p_menu->m_secondaryTopLeft[1];
	}
	else {
		LegoFloat maximum[2] = {0.0f, 0.0f};
		for (LegoS32 i = 0; i < 6; ++i) {
			if (((GlobalOverlayRecord*) p_menu->m_overlayRecords[i])->m_text[0] != 0) {
				LayoutMenuOptionWidgets(p_menu, NULL, 0, p_menu->m_unk0x19c, i, origin, size);
				if (maximum[0] < size[0]) {
					maximum[0] = size[0];
				}
				if (maximum[1] < size[1]) {
					maximum[1] = size[1];
				}
			}
		}
		size[0] = maximum[0];
		size[1] = maximum[1];
		p_menu->m_secondaryTopLeft[0] = p_menu->m_secondaryBottomRight[0] = origin[0];
		p_menu->m_secondaryTopLeft[1] = p_menu->m_secondaryBottomRight[1] = origin[1];
		p_menu->m_secondaryBottomRight[0] += maximum[0];
		p_menu->m_secondaryBottomRight[1] += maximum[1];
	}
	LayoutMenuOptionWidgets(p_menu, NULL, 1, p_menu->m_unk0x19c, GetMenuSelectedOption(p_menu), origin, size);
}

// STUB: LEGORACERS2 0x0041de40
LegoBool32 __fastcall CanMenuCancel(MenuManager* p_menu, void*)
{
	return p_menu->m_unk0x1e8 != 0 && (LegoS32) p_menu->m_unk0x198 > 1 &&
		   *(LegoU32*) ((LegoU8*) p_menu + 0x4c + p_menu->m_unk0x198 * 0x14) == 0;
}

// STUB: LEGORACERS2 0x00422b30
LegoBool32 __fastcall ProcessMenuWidgetsInput(
	MenuManager* p_menu,
	void*,
	const SecondaryManagerMediaSlot* p_input,
	LegoS32* p_changed,
	LegoS32* p_sound
)
{
	LegoBool32 handled = FALSE;
	LegoS32 selected = *(LegoS32*) ((LegoU8*) p_menu + (p_menu->m_unk0x198 + 4) * 0x14);
	*p_changed = 0;
	*p_sound = -1;
	for (LegoS32 i = 0; i < 18; ++i) {
		MenuWidgetBase* widget = p_menu->m_widgets[i];
		if (widget == NULL || widget->m_state == 0) {
			continue;
		}
		SecondaryManagerMediaSlot input = {0, 0, 0, 0};
		if (widget->m_initialValue == selected) {
			input = *p_input;
		}
		LegoS32 sound = -1;
		LegoU32 result = widget->m_vtable->m_unk0x10(widget, NULL, (LegoU32*) &sound, (const LegoU32*) &input);
		if (result == 1) {
			if (*(LegoU32*) ((LegoU8*) p_menu + 0x4c + p_menu->m_unk0x198 * 0x14) == 0) {
				PopSecondaryMenuActions(p_menu, NULL, 1, 1);
				handled = TRUE;
			}
			else {
				sound = 2;
			}
		}
		else if (result == 2) {
			ApplyMenuConfirmAction(p_menu, NULL);
			handled = TRUE;
		}
		*p_changed = 1;
		if (sound != -1) {
			*p_sound = sound;
		}
	}
	return handled;
}

// STUB: LEGORACERS2 0x00422c60
void __fastcall UpdateMenuInteractionWidgets(MenuManager* p_menu, void*, const SecondaryManagerMediaSlot* p_input)
{
	if (p_menu->m_unk0x198 == 0 || p_menu->m_unk0x1a8 != 0) {
		return;
	}
	if (p_menu->m_unk0x19c == 22) {
		RefreshMenuNetworkList(p_menu, NULL);
	}
	else if (p_menu->m_unk0x19c == 23) {
		for (LegoS32 i = 14; i < 17; ++i) {
			MenuWidgetBase* widget = p_menu->m_widgets[i];
			if (widget->m_state != 0) {
				LegoU32 output;
				widget->m_vtable->m_unk0x10(widget, NULL, &output, (const LegoU32*) p_input);
			}
		}
		if ((p_menu->m_unk0x1e0 & 1) != 0) {
			LegoU8* network = (LegoU8*) g_gameController.m_networkApp;
			if ((*(LegoU32*) (network + 0x60) != 2 && *(LegoU32*) (network + 0x64) != 4) ||
				*(LegoU32*) (network + 0x2a84) == 0) {
				SetMenuManagerInputEnabled(p_menu, NULL, 1);
			}
		}
	}
	else if (p_menu->m_unk0x19c == 26) {
		MenuWidgetBase* widget = p_menu->m_widgets[17];
		LegoU32 output = (LegoU32) p_input;
		LegoU32 result = widget->m_vtable->m_unk0x10(widget, NULL, &output, (const LegoU32*) p_input);
		if (result == 2) {
			ApplyMenuConfirmAction(p_menu, NULL);
		}
		if ((LegoS32) output != -1) {
			PlayMenuInteractionSound(output);
		}
		if ((p_menu->m_unk0x1e0 & 1) != 0 && *(LegoU32*) ((LegoU8*) widget + 0x48) == 0) {
			SetMenuManagerInputEnabled(p_menu, NULL, 1);
		}
	}
}

// STUB: LEGORACERS2 0x00422f50
void __fastcall SetMenuManagerInputEnabled(MenuManager* p_menu, void*, LegoU32 p_enabled)
{
	if (p_enabled != 0) {
		p_menu->m_unk0x1e0 &= ~1u;
	}
	else {
		p_menu->m_unk0x1e0 |= 1;
	}
	HighlightMenuOption(p_menu, NULL, GetMenuSelectedOption(p_menu), 1);
}

// STUB: LEGORACERS2 0x00422fa0
void __fastcall SetMenuManagerWaitForInput(MenuManager* p_menu, void*, LegoU32 p_wait)
{
	if (p_wait == 0) {
		p_menu->m_unk0x1e0 &= ~2u;
	}
	else {
		p_menu->m_unk0x1e0 |= 2;
	}
	SetMenuManagerInputEnabled(p_menu, NULL, p_wait == 0);
}

// STUB: LEGORACERS2 0x00423000
void __stdcall PlayMenuInteractionSound(LegoS32 p_sound)
{
	SecondaryManager* secondary = (SecondaryManager*) g_gameController.m_secondaryManager;
	AudioSourceRecord* sound = (AudioSourceRecord*) secondary->m_unk0x154[p_sound];
	sound->m_vtable->m_resetPlaybackState(sound, NULL);
}

// STUB: LEGORACERS2 0x00423020
LegoBool32 __fastcall IsMenuOptionEnabled(MenuManager* p_menu, void*, LegoS32 p_option)
{
	if (p_option < 0 || p_option >= 6) {
		return FALSE;
	}
	GlobalOverlayRecord* record = (GlobalOverlayRecord*) p_menu->m_overlayRecords[p_option];
	return record->m_text[0] != 0 && (p_option > 4 || *(LegoU32*) ((LegoU8*) p_menu + 0x1f0 + p_option * 4) == 0);
}

// STUB: LEGORACERS2 0x004235a0
void __fastcall HighlightMenuOption(MenuManager* p_menu, void*, LegoS32 p_option, LegoU32 p_selected)
{
	GlobalOverlayRecord* record = ((GlobalOverlayRecord**) p_menu->m_overlayRecords)[p_option];
	LegoU32 color;
	if (p_selected == 0) {
		color = -1;
	}
	else {
		color = (~p_menu->m_unk0x218 & 1) == 0 ? g_menuChoiceColorA : g_menuChoiceColorB;
	}
	record->SetColor(&color);
	if (p_selected != 0 && (LegoS32) p_menu->m_unk0x22c > 1 && (p_menu->m_unk0x1e0 & 1) == 0) {
		record->m_animationFlags |= 0x40;
	}
	else {
		record->m_animationFlags &= ~0x40u;
	}
}

// STUB: LEGORACERS2 0x00423a00
void __fastcall ReleaseSecondaryObject8c(void* p_object, void*)
{
	GlobalOverlayRecord** records = (GlobalOverlayRecord**) p_object;
	for (LegoS32 index = 1; index >= 0; --index) {
		if (records[index] != NULL) {
			records[index]->Release();
			records[index] = NULL;
		}
	}
}

// STUB: LEGORACERS2 0x004b9370
void __fastcall MergeSecondaryMediaSlot(
	SecondaryManagerMediaSlot* p_destination,
	void*,
	const SecondaryManagerMediaSlot* p_source
)
{
	p_destination->m_resourceId |= p_source->m_resourceId;
	p_destination->m_resourceChangeMask |= p_source->m_resourceChangeMask;
	p_destination->m_state |= p_source->m_state;
	p_destination->m_stateChangeMask |= p_source->m_stateChangeMask;
}
