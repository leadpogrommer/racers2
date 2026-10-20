#include "secondarymenuactions.h"

#include "controllerrecordingservice.h"
#include "gamecontroller.h"
#include "gamecontrollerconfig.h"
#include "gamecontrollersession.h"
#include "gamerandom.h"
#include "globaloverlaymanager.h"
#include "menuinputbindings.h"
#include "menutextentry.h"
#include "menutrackselector.h"
#include "secondarymanager.h"

#include <string.h>

void __fastcall PushMenuManagerActionState(MenuManager*, void*, LegoU32, LegoU32, LegoU32);
void __fastcall BeginMenuManagerChoice(
	MenuManager*,
	void*,
	const LegoFloat*,
	LegoU32,
	LegoU32,
	LegoU32,
	LegoU32,
	LegoU32
);
void __fastcall PopSecondaryMenuActions(MenuManager*, void*, LegoU32, LegoS32);
void __fastcall ResetSecondaryMenuAction(MenuManager*, void*, LegoU32);
void __fastcall BeginMenuTransition(GameObject*, void*, LegoU32);
void __fastcall UpdateMenuManagerCurrentAction(MenuManager*, void*);
void __fastcall SetMenuManagerInputEnabled(MenuManager*, void*, LegoU32);
void __stdcall PlayMenuInteractionSound(LegoS32);
void __fastcall HighlightMenuOption(MenuManager*, void*, LegoS32, LegoU32);
LegoU32 __fastcall GetMenuManagerInitialValue(MenuManager*, void*, LegoU32);
LegoBool32 __stdcall IsMenuPlayerAvailable(LegoU32);
void __fastcall StartNewMenuSession(void*, void*);
void __fastcall IncrementMenuModeProgress(void*, void*, LegoU32);
void __fastcall SetMenuExitAction(void*, void*, LegoU32);
void __fastcall BeginSecondarySave(void*, void*);
void __fastcall BeginSecondaryLoad(void*, void*);
void __fastcall SetSecondarySaveSlot(void*, void*, LegoU32);
void __fastcall SetControllerMenuMode(void*, void*, LegoU32);
LegoBool32 __fastcall HostMenuNetworkSession(void*, void*);
void __fastcall LeaveMenuNetworkHost(void*, void*);
void __fastcall LeaveMenuNetworkBrowser(void*, void*);
LegoBool32 __fastcall JoinMenuNetworkSession(void*, void*, LegoS32);
void __fastcall StartMenuNetworkGame(void*, void*);
LegoBool32 __fastcall IsFoyerModeUnlocked(void*, void*, LegoU32);
extern LegoU32 g_menuDisabledColor;
extern const LegoU32 g_secondarySessionRaceValues[26];

// STUB: LEGORACERS2 0x0041ddb0
void __fastcall RestoreMenuManagerAction(MenuManager* p_menu, void*, LegoU32 p_unused, LegoS32 p_action)
{
	LegoU8* menu = (LegoU8*) p_menu;
	LegoS32* depth = (LegoS32*) (menu + 0x198);
	if (p_action != -1) {
		LegoS32 index = *depth;
		LegoU32* record = (LegoU32*) (menu + 0x44 + index * 0x14);
		while ((LegoS32) record[0] != p_action) {
			--index;
			if (index < 1 || record[2] != 0) {
				return;
			}
			record -= 5;
		}
		*depth = index;
	}
	*(LegoU32*) (menu + 0x1a8) = 1;
	*(LegoU32*) (menu + 0x1ac) = 0;
	*(LegoU32*) (menu + 0x1a0) = *depth < 1 ? 0xffffffff : *(LegoU32*) (menu + 0x44 + *depth * 0x14);
	*(LegoU32*) (menu + 0x1a4) = 0;
}

// STUB: LEGORACERS2 0x00420d30
LegoBool32 __fastcall ApplyMenuConfirmAction(MenuManager* p_menu, void*)
{
	const LegoU32 menuAddress = (LegoU32) p_menu;
	const LegoU32 sessionAddress = (LegoU32) GetGameControllerSession();
	const LegoU32 settingsAddress = (LegoU32) GetGameControllerSettings();
	const LegoU32 networkAddress = (LegoU32) g_gameController.m_networkApp;
	const LegoU32 secondaryAddress = (LegoU32) g_gameController.m_secondaryManager;
	LegoU32 world;
	LegoS32 selected;
	LegoU32 player;
	LegoS32 value;
	LegoS32 nextRandom;
	LegoS32 index;
	LegoU32 argument;
	LegoU32* source;
	LegoS32* availableWorld;
	LegoU32* destination;
	bool accept;
	GameControllerName temporaryName;
	LegoU32 result;
	LegoS32 widgetResult;
	LegoS32 scratch[26];
	LegoU8 widgetScratch[24];

	index = 0;
	value = *(LegoS32*) (menuAddress + 0x198);
	if (value == 0) {
		return 0;
	}
	selected = *(LegoS32*) (menuAddress + (value * 5 + 0x14) * 4);
	argument = 1;
	result = 1;
	switch (*(LegoU32*) (menuAddress + 0x44 + value * 0x14)) {
	case 0:
		switch (selected) {
		case 0:
		case 1:
		case 2:
		case 3:
			goto switchD_00420d7d_default;
		}
		break;
	case 1:
		switch (selected) {
		case 0:
			value = IsMenuPlayerAvailable(1);
			if (value != 0) {
				PushMenuManagerActionState(p_menu, NULL, 2, 0xffffffff, 0);
				DisableSecondaryInactivityTimer(g_gameController.m_unk0xc8, NULL);
				return 1;
			}
			break;
		case 1:
			value = IsMenuPlayerAvailable(3);
			if (value != 0) {
				PushMenuManagerActionState(p_menu, NULL, 0x13, 0xffffffff, 0);
				DisableSecondaryInactivityTimer(g_gameController.m_unk0xc8, NULL);
				return 1;
			}
			break;
		case 2:
			PushMenuManagerActionState(p_menu, NULL, 0x18, 0xffffffff, 0);
			DisableSecondaryInactivityTimer(g_gameController.m_unk0xc8, NULL);
			return 1;
		case 3:
			PushMenuManagerActionState(p_menu, NULL, 5, 0xffffffff, 0);
			DisableSecondaryInactivityTimer(g_gameController.m_unk0xc8, NULL);
			return 1;
		case 4:
			PushMenuManagerActionState(p_menu, NULL, 6, 0xffffffff, 0);
			DisableSecondaryInactivityTimer(g_gameController.m_unk0xc8, NULL);
			return 1;
		case 5:
			*(LegoU32*) (menuAddress + 0x214) = 0;
			scratch[7] = 0x3f000000;
			scratch[8] = 0x3f000000;
			BeginMenuManagerChoice(
				p_menu,
				NULL,
				(const LegoFloat*) scratch + 7,
				0,
				0xffffffff,
				0xffffffff,
				0x348,
				0x1b
			);
			DisableSecondaryInactivityTimer(g_gameController.m_unk0xc8, NULL);
			return 1;
		default:
			DisableSecondaryInactivityTimer(g_gameController.m_unk0xc8, NULL);
			return 0;
		}
		PlayMenuInteractionSound(2);
		DisableSecondaryInactivityTimer(g_gameController.m_unk0xc8, NULL);
		return 0;
	case 2:
		switch (selected) {
		case 0:
		switchD_004212cc_caseD_3:
			*(LegoU32*) (sessionAddress + 4) = 1;
			PushMenuManagerActionState(p_menu, NULL, 0x19, 0xffffffff, 0);
			return 1;
		case 1:
		switchD_004212cc_caseD_0:
			*(LegoU32*) (sessionAddress + 8) = 0;
		LAB_00420f40:
			PushMenuManagerActionState(p_menu, NULL, 0x1b, 0xffffffff, 0);
			return 1;
		case 2:
			*(LegoU32*) (sessionAddress + 8) = 0xffffffff;
			PushMenuManagerActionState(p_menu, NULL, 0x1b, 0xffffffff, 0);
			return 1;
		case 3:
			*(LegoU32*) (sessionAddress + 8) = 5;
			PushMenuManagerActionState(p_menu, NULL, 0x1b, 0xffffffff, 0);
			return 1;
		case 4:
			*(LegoU32*) (sessionAddress + 8) = 4;
		LAB_00420fb7:
			PushMenuManagerActionState(p_menu, NULL, 0x1b, 0xffffffff, 0);
			return 1;
		case 5:
			goto switchD_004213b8_caseD_5;
		default:
			break;
		}
	case 3:
		if (selected == 0) {
			PushMenuManagerActionState(p_menu, NULL, 0x13, 0xffffffff, 0);
			return 1;
		}
		if (selected == 1) {
			PushMenuManagerActionState(p_menu, NULL, 0x14, 0xffffffff, 0);
			return 1;
		}
		accept = selected == 5;
		goto LAB_004211f8;
	case 4:
		if (selected == 0) {
			goto LAB_00421d26;
		}
		if (selected == 1) {
			goto LAB_00420fb7;
		}
		accept = selected == 5;
		goto LAB_004211f8;
	case 5:
		switch (selected) {
		case 0:
			PushMenuManagerActionState(p_menu, NULL, 8, 0xffffffff, 0);
			return 1;
		case 1:
			PushMenuManagerActionState(p_menu, NULL, 9, 0xffffffff, 0);
			return 1;
		case 2:
			PushMenuManagerActionState(p_menu, NULL, 10, 0xffffffff, 0);
			return 1;
		case 3:
			PushMenuManagerActionState(p_menu, NULL, 0xb, 0xffffffff, 0);
			return 1;
		case 4:
			PushMenuManagerActionState(p_menu, NULL, 0xc, 0xffffffff, 0);
			return 1;
		case 5:
			goto switchD_004213b8_caseD_5;
		}
		break;
	case 6:
		if (selected == 0) {
			*(LegoU32*) (menuAddress + 0x204) = 1;
			PushMenuManagerActionState(p_menu, NULL, 0x2a, 0xffffffff, 0);
			return 1;
		}
		if (selected == 1) {
			goto LAB_0042203b;
		}
		accept = selected == 5;
		goto LAB_004211f8;
	case 7:
		if (-1 < selected) {
			if (selected < 2) {
				SetSecondarySaveSlot(g_gameController.m_secondaryManager, NULL, selected + 1);
				PushMenuManagerActionState(p_menu, NULL, 0x2a, 0xffffffff, 0);
				return 1;
			}
		LAB_004211f5:
			accept = selected == 5;
			goto LAB_004211f8;
		}
		break;
	case 8:
		switch (selected) {
		case 0:
			PushMenuManagerActionState(p_menu, NULL, 0xd, 0xffffffff, 0);
			return 1;
		case 1:
			PushMenuManagerActionState(p_menu, NULL, 0xe, 0xffffffff, 0);
			return 1;
		case 2:
			PushMenuManagerActionState(p_menu, NULL, 0xf, 0xffffffff, 0);
			return 1;
		case 5:
			goto switchD_004213b8_caseD_5;
		}
		break;
	case 9:
		if (-1 < selected) {
			if (selected < 2) {
				value = IsMenuPlayerAvailable(selected + 1);
				if (value != 0) {
					*(LegoS32*) (menuAddress + 0x21c) = selected + 1;
					goto LAB_004211ad;
				}
				goto LAB_004211ce;
			}
			goto LAB_004211f5;
		}
		break;
	case 10:
		if ((-1 < selected) && (3 < selected)) {
			goto LAB_004211f5;
		}
		break;
	case 0xb:
		if (selected == 0) {
			value = GetSecondaryPendingMenuAction(p_menu, NULL);
			if (value == 0xb) {
				ResetSecondaryMenuAction(p_menu, NULL, 1);
				return 1;
			}
			goto LAB_004211ff;
		}
		break;
	case 0xc:
	case 0xd:
	case 0xe:
	case 0xf:
	case 0x12:
	case 0x26:
	case 0x27:
		accept = selected == 0;
		goto LAB_004211f8;
	case 0x10:
		if (selected == 0) {
		LAB_004211ad:
			PushMenuManagerActionState(p_menu, NULL, 0x11, 0xffffffff, 0);
			return 1;
		}
		if (selected == 1) {
			PushMenuManagerActionState(p_menu, NULL, 0x12, 0xffffffff, 0);
			return 1;
		}
		accept = selected == 5;
		goto LAB_004211f8;
	case 0x11:
		if ((selected == 0) &&
			(value = CommitMenuInputBindings(
				 p_menu,
				 NULL,
				 0,
				 (LegoU32*) (settingsAddress + (~*(LegoU32*) (menuAddress + 0x218) & 1) * 0xac + 0xb4)
			 ),
			 value != 0)) {
			PopSecondaryMenuActions(p_menu, NULL, 1, 1);
			return 1;
		}
		break;
	case 0x13:
		switch (selected) {
		case 0:
			goto switchD_004212cc_caseD_0;
		case 1:
			value = IsMenuPlayerAvailable(1);
			if (value != 0) {
				*(LegoU32*) (sessionAddress + 8) = 2;
				*(LegoU32*) (menuAddress + 0x21c) = 1;
				PushMenuManagerActionState(p_menu, NULL, 0x1f, 0xffffffff, 0);
				*(LegoU32*) (sessionAddress + 0x1a6c) = 0;
				*(LegoU32*) (sessionAddress + 0x1a70) = 0;
				return 1;
			}
			goto LAB_004211ce;
		case 2:
		switchD_004213b8_caseD_2:
			*(LegoU32*) (sessionAddress + 8) = 3;
			goto LAB_00420f40;
		case 3:
			goto switchD_004212cc_caseD_3;
		case 4:
			*(LegoU32*) (sessionAddress + 8) = 4;
			PushMenuManagerActionState(p_menu, NULL, 0x1b, 0xffffffff, 0);
			return 1;
		case 5:
			goto switchD_004213b8_caseD_5;
		}
		break;
	case 0x14:
		if (selected == 0) {
			PushMenuManagerActionState(p_menu, NULL, 0x15, 0xffffffff, 0);
			value = networkAddress;
			*(LegoU32*) (networkAddress + 0x5c) = 1;
			*(LegoU32*) (value + 0x60) = 0;
			return 1;
		}
		if (selected == 1) {
			PushMenuManagerActionState(p_menu, NULL, 0x1f, 0xffffffff, 0);
			value = networkAddress;
			*(LegoU32*) (networkAddress + 0x5c) = 2;
			*(LegoU32*) (value + 100) = 0;
			return 1;
		}
		accept = selected == 5;
		goto LAB_004211f8;
	case 0x15:
		switch (selected) {
		case 0:
			*(LegoU32*) (sessionAddress + 8) = 0;
			PushMenuManagerActionState(p_menu, NULL, 0x1b, 0xffffffff, 0);
			return 1;
		case 1:
			*(LegoU32*) (sessionAddress + 8) = 2;
			goto LAB_00421e08;
		case 2:
			goto switchD_004213b8_caseD_2;
		case 5:
			goto switchD_004213b8_caseD_5;
		}
		break;
	case 0x16:
		if (-1 < selected) {
			if (selected < 5) {
				value = JoinMenuNetworkSession(g_gameController.m_networkApp, NULL, selected);
				if (value == 0) {
					return 1;
				}
				PushMenuManagerActionState(p_menu, NULL, 0x17, 0xffffffff, 0);
				return 1;
			}
			if (selected == 5) {
				LeaveMenuNetworkBrowser(g_gameController.m_networkApp, NULL);
				goto switchD_004213b8_caseD_5;
			}
		}
		break;
	case 0x17:
		if (((*(LegoS32*) (networkAddress + 0x60) == 2) || (*(LegoS32*) (networkAddress + 100) == 4)) &&
			(*(LegoS32*) (networkAddress + 0x2a84) != 0)) {
			return 1;
		}
		switch (selected) {
		case 0:
			StartMenuNetworkGame(g_gameController.m_networkApp, NULL);
			return 1;
		case 1:
			value = g_controllerRecordingService->m_vtable->m_getUnknownWord0xd8(g_controllerRecordingService, NULL);
			g_controllerRecordingService->m_vtable
				->m_setUnknownWord0xd8(g_controllerRecordingService, NULL, value == 0);
			if (value != 0) {
				((GlobalOverlayRecord*) p_menu->m_overlayRecords[1])->InitializeTextByIndex(0x10b, 0x41, 0);
				return result;
			}
			((GlobalOverlayRecord*) p_menu->m_overlayRecords[1])->InitializeTextByIndex(0xf3, 0x41, 0);
			return result;
		case 2:
			*(LegoU32*) (networkAddress + 0x2a84) = 1;
			SetMenuManagerInputEnabled(p_menu, NULL, 0);
			return 1;
		case 5:
			if (*(LegoS32*) (networkAddress + 0x5c) != 1) {
				LeaveMenuNetworkBrowser(g_gameController.m_networkApp, NULL);
				PopSecondaryMenuActions(p_menu, NULL, 1, 1);
				return 1;
			}
			LeaveMenuNetworkHost(g_gameController.m_networkApp, NULL);
			PopSecondaryMenuActions(p_menu, NULL, 1, 1);
			return 1;
		}
		break;
	case 0x18:
		if (-1 < selected) {
			if (selected < 2) {
				value = IsMenuPlayerAvailable(selected + 1);
				if (value != 0) {
					*(LegoS32*) (menuAddress + 0x21c) = selected + 1;
					PushMenuManagerActionState(p_menu, NULL, 4, 0xffffffff, 0);
					return 1;
				}
				goto LAB_004211ce;
			}
			goto LAB_004211f5;
		}
		break;
	case 0x19:
		if (selected == 0) {
			StartNewMenuSession(g_gameController.m_unk0x8c, NULL);
			PushMenuManagerActionState(p_menu, NULL, 0x1f, 0xffffffff, 0);
			return 1;
		}
		if (selected == 1) {
			*(LegoU32*) (menuAddress + 0x214) = 1;
			scratch[0xb] = 0x3f000000;
			scratch[0xc] = 0x3f0ccccd;
			BeginMenuManagerChoice(
				p_menu,
				NULL,
				(const LegoFloat*) scratch + 0xb,
				0,
				0xffffffff,
				0xffffffff,
				0x1b,
				0xffffffff
			);
			return 1;
		}
		accept = selected == 5;
	LAB_004211f8:
		if (accept) {
		switchD_004213b8_caseD_5:
		LAB_004211ff:
			PopSecondaryMenuActions(p_menu, NULL, 1, 1);
			return 1;
		}
		break;
	case 0x1a:
		switch (selected) {
		case 0:
			value = IsMenuPlayerAvailable(3);
			if (value != 0) {
				*(LegoU32*) (menuAddress + 0x214) = 2;
				scratch[9] = 0x3f000000;
				scratch[10] = 0x3f000000;
				BeginMenuManagerChoice(
					p_menu,
					NULL,
					(const LegoFloat*) scratch + 9,
					1,
					0xffffffff,
					0xffffffff,
					0x1b,
					0xffffffff
				);
				return 1;
			}
			goto LAB_004211ce;
		case 1:
			if (9 < *(LegoS32*) (sessionAddress + 0x1a6c)) {
				return 1;
			}
			*(LegoS32*) (sessionAddress + 0x1a70) = *(LegoS32*) (sessionAddress + 0x1a6c);
			PushMenuManagerActionState(p_menu, NULL, 0x1b, 0, 0);
			return 1;
		case 2:
			value = *(LegoS32*) (sessionAddress + 0x1a6c);
			if (0 < value) {
				if ((*(LegoU8*) (menuAddress + 0x1e0) & 1) == 0) {
					((MenuTrackSelector*) p_menu->m_widgets[17])->SetActive(1);
					SetMenuManagerInputEnabled(p_menu, NULL, 0);
				}
				else {
					value = *(LegoS32*) (*(LegoS32*) (menuAddress + 0x2b0) + 0x4c);
					*(LegoS32*) (sessionAddress + 0x1a6c) = *(LegoS32*) (sessionAddress + 0x1a6c) + -1;
					if (value < *(LegoS32*) (sessionAddress + 0x1a6c)) {
						index = value * 4 + 0x1a78;
						do {
							value = value + 1;
							*(LegoU32*) (sessionAddress + index) = *(LegoU32*) (sessionAddress + index + 4);
							*(LegoU32*) (sessionAddress + index + 0x28) = *(LegoU32*) (sessionAddress + index + 0x2c);
							*(LegoU32*) (sessionAddress + index + 0x50) = *(LegoU32*) (sessionAddress + index + 0x54);
							index = index + 4;
						} while (value < *(LegoS32*) (sessionAddress + 0x1a6c));
					}
					scratch[0x15] = 0;
					scratch[0x14] = 0;
					scratch[0x13] = 0;
					scratch[0x12] = 0;
					p_menu->m_widgets[17]->m_vtable->m_unk0x10(
						p_menu->m_widgets[17],
						NULL,
						(LegoU32*) &widgetResult,
						(const LegoU32*) (scratch + 0x12)
					);
				}
				value = *(LegoS32*) (sessionAddress + 0x1a6c);
			}
			if (value != 0) {
				*(LegoU32*) (menuAddress + 500) = 0;
				scratch[4] = 0xffffffff;
				((GlobalOverlayRecord*) p_menu->m_overlayRecords[1])->SetColor((const LegoU32*) scratch + 4);
				*(LegoU32*) (menuAddress + 0x200) = 0;
				scratch[3] = 0xffffffff;
				((GlobalOverlayRecord*) p_menu->m_overlayRecords[4])->SetColor((const LegoU32*) scratch + 3);
				return 1;
			}
			*(LegoU32*) (menuAddress + (*(LegoS32*) (menuAddress + 0x198) + 4) * 0x14) = 1;
			HighlightMenuOption(p_menu, NULL, 2, 0);
			HighlightMenuOption(p_menu, NULL, 1, 1);
			*(LegoU32*) (menuAddress + 0x1f0) = 1;
			((GlobalOverlayRecord*) p_menu->m_overlayRecords[0])->SetColor((const LegoU32*) &g_menuDisabledColor);
			*(LegoU32*) (menuAddress + 0x1f8) = 1;
			((GlobalOverlayRecord*) p_menu->m_overlayRecords[2])->SetColor((const LegoU32*) &g_menuDisabledColor);
			*(LegoU32*) (menuAddress + 0x1fc) = 1;
			((GlobalOverlayRecord*) p_menu->m_overlayRecords[3])->SetColor((const LegoU32*) &g_menuDisabledColor);
			return 1;
		case 3:
			if (0 < *(LegoS32*) (sessionAddress + 0x1a6c)) {
				if ((*(LegoU8*) (menuAddress + 0x1e0) & 1) == 0) {
					((MenuTrackSelector*) p_menu->m_widgets[17])->SetActive(1);
					SetMenuManagerInputEnabled(p_menu, NULL, 0);
					return 1;
				}
				*(LegoU32*) (sessionAddress + 0x1a70) = *(LegoU32*) (*(LegoS32*) (menuAddress + 0x2b0) + 0x4c);
				PushMenuManagerActionState(p_menu, NULL, 0x1b, 0, 0);
				return 1;
			}
		case 4:
			if (*(LegoS32*) (sessionAddress + 0x1a6c) < 10) {
				scratch[0] = 0;
				scratch[1] = 0;
				scratch[0xd] = 0;
				scratch[0xe] = 0;
				scratch[0xf] = 0;
				scratch[0x10] = 0;
				scratch[0x11] = 0;
				do {
					value = 0;
					if (index == 0) {
						widgetResult = 0;
						availableWorld = (LegoS32*) (scratch + 0xd);
						do {
							selected = IsFoyerModeUnlocked(g_gameController.m_unk0x8c, NULL, value);
							if (selected != 0) {
								*availableWorld = value;
								widgetResult = widgetResult + 1;
								availableWorld = availableWorld + 1;
							}
							value = value + 1;
							selected = widgetResult;
						} while (value < 5);
					}
					else {
						selected = (scratch[0] != 0) + 4;
					}
					value = (LegoS32) GetGameRandomState();
					nextRandom = *(LegoS32*) (value + 0x3c) * *(LegoS32*) (value + 0x34) + *(LegoS32*) (value + 0x38);
					*(LegoS32*) (value + 0x3c) = nextRandom;
					scratch[index] = (LegoS32) ((nextRandom >> 8 & 0xffffffU) * selected) >> 0x18;
					index = index + 1;
				} while (index < 2);
				*(LegoS32*) (sessionAddress + *(LegoS32*) (sessionAddress + 0x1a6c) * 4 + 0x1a78) =
					scratch[scratch[0] + 0xd];
				*(LegoS32*) (sessionAddress + *(LegoS32*) (sessionAddress + 0x1a6c) * 4 + 0x1aa0) = scratch[1] + 1;
				value = *(LegoS32*) (sessionAddress + 0x1a6c);
				*(LegoU32*) (sessionAddress + value * 4 + 0x1ac8) = g_secondarySessionRaceValues
					[*(LegoS32*) (sessionAddress + value * 4 + 0x1a78) * 5 +
					 *(LegoS32*) (sessionAddress + value * 4 + 0x1aa0)];
				*(LegoS32*) (sessionAddress + 0x1a6c) = *(LegoS32*) (sessionAddress + 0x1a6c) + 1;
				scratch[0x19] = 0;
				scratch[0x18] = 0;
				scratch[0x17] = 0;
				scratch[0x16] = 0;
				p_menu->m_widgets[17]->m_vtable->m_unk0x10(
					p_menu->m_widgets[17],
					NULL,
					(LegoU32*) widgetScratch,
					(const LegoU32*) (scratch + 0x16)
				);
				if (*(LegoS32*) (sessionAddress + 0x1a6c) < 10) {
					*(LegoU32*) (menuAddress + 0x1f0) = 0;
					scratch[6] = 0xffffffff;
					((GlobalOverlayRecord*) p_menu->m_overlayRecords[0])->SetColor((const LegoU32*) scratch + 6);
					*(LegoU32*) (menuAddress + 0x1f8) = 0;
					scratch[5] = 0xffffffff;
					((GlobalOverlayRecord*) p_menu->m_overlayRecords[2])->SetColor((const LegoU32*) scratch + 5);
					*(LegoU32*) (menuAddress + 0x1fc) = 0;
					scratch[2] = 0xffffffff;
					((GlobalOverlayRecord*) p_menu->m_overlayRecords[3])->SetColor((const LegoU32*) scratch + 2);
					return 1;
				}
			}
			*(LegoU32*) (menuAddress + (*(LegoS32*) (menuAddress + 0x198) + 4) * 0x14) = 0;
			HighlightMenuOption(p_menu, NULL, 4, 0);
			HighlightMenuOption(p_menu, NULL, 0, 1);
			*(LegoU32*) (menuAddress + 0x200) = 1;
			((GlobalOverlayRecord*) p_menu->m_overlayRecords[4])->SetColor((const LegoU32*) &g_menuDisabledColor);
			*(LegoU32*) (menuAddress + 500) = 1;
			((GlobalOverlayRecord*) p_menu->m_overlayRecords[1])->SetColor((const LegoU32*) &g_menuDisabledColor);
			return 1;
		case 5:
			if (*(LegoS32*) sessionAddress != 2) {
				RestoreMenuManagerAction(p_menu, NULL, 0, 0x13);
				return 1;
			}
			RestoreMenuManagerAction(p_menu, NULL, 0, 0x15);
			return 1;
		}
		break;
	case 0x1b:
		if (-1 < selected) {
			if (selected < 5) {
				switch (selected) {
				case 1:
					world = 2;
					break;
				case 2:
					world = 3;
					break;
				case 3:
					world = 1;
					break;
				case 4:
					world = 4;
					break;
				default:
					world = 0;
				}
				if (*(LegoS32*) (sessionAddress + 8) == 2) {
					*(LegoU32*) (sessionAddress + *(LegoS32*) (sessionAddress + 0x1a70) * 4 + 0x1a78) = world;
				}
				else {
					*(LegoU32*) (sessionAddress + 0xc) = world;
				}
				*(LegoU32*) (settingsAddress + 0x7f9c) = world;
				if (*(LegoS32*) (sessionAddress + 8) != 3) {
					if (*(LegoS32*) (sessionAddress + 8) == 4) {
						PushMenuManagerActionState(p_menu, NULL, 0x1e, 0xffffffff, 0);
						return 1;
					}
					value = GetMenuParentAction(p_menu, NULL);
					if (value != 4) {
						PushMenuManagerActionState(p_menu, NULL, 0x1c, 0xffffffff, 0);
						return 1;
					}
					BeginMenuTransition(p_menu, NULL, 0);
					*(LegoU32*) (menuAddress + 0x1d8) = 3;
					return 1;
				}
				goto LAB_00421cbc;
			}
			goto LAB_004211f5;
		}
		break;
	case 0x1c:
		if (-1 < selected) {
			if (selected < 5) {
				if (*(LegoS32*) (sessionAddress + 8) == -1) {
					return 1;
				}
				selected = selected + 1;
				if (*(LegoS32*) (sessionAddress + 8) == 2) {
					value = *(LegoS32*) (sessionAddress + *(LegoS32*) (sessionAddress + 0x1a70) * 4 + 0x1a78);
					*(LegoS32*) (sessionAddress + *(LegoS32*) (sessionAddress + 0x1a70) * 4 + 0x1aa0) = selected;
				}
				else {
					value = *(LegoS32*) (sessionAddress + 0xc);
					*(LegoS32*) (sessionAddress + 0x10) = selected;
				}
				*(LegoS32*) (settingsAddress + value * 4 + 0x7fa0) = selected;
				if (*(LegoS32*) (sessionAddress + 8) != 5) {
					PushMenuManagerActionState(p_menu, NULL, 0x1d, 0xffffffff, 0);
					return 1;
				}
				PushMenuManagerActionState(p_menu, NULL, 0x1f, 0xffffffff, 0);
				return 1;
			}
			goto LAB_004211f5;
		}
		break;
	case 0x1d:
		if (selected == 0) {
			if (*(LegoS32*) (sessionAddress + 8) == 2) {
				UpdateMenuManagerCurrentAction(p_menu, NULL);
				*(LegoS32*) (sessionAddress + 0x1a70) = *(LegoS32*) (sessionAddress + 0x1a70) + 1;
				if (*(LegoS32*) (sessionAddress + 0x1a6c) < *(LegoS32*) (sessionAddress + 0x1a70)) {
					*(LegoS32*) (sessionAddress + 0x1a6c) = *(LegoS32*) (sessionAddress + 0x1a70);
				}
				RestoreMenuManagerAction(p_menu, NULL, 0, 0x1a);
				return 1;
			}
		LAB_00421cbc:
			value = IsMenuPlayerAvailable(1);
		joined_r0x00421e06:
			if (value != 0) {
			LAB_00421e08:
				*(LegoU32*) (menuAddress + 0x21c) = argument;
				PushMenuManagerActionState(p_menu, NULL, 0x1f, 0xffffffff, 0);
				return 1;
			}
			goto LAB_004211ce;
		}
		break;
	case 0x1e:
		if (-1 < selected) {
			if (selected < 2) {
				*(LegoS32*) (sessionAddress + 0x10) = selected + 1;
				goto LAB_00421cbc;
			}
			goto LAB_004211f5;
		}
		break;
	case 0x1f:
		result = 0;
		if (selected != 0) {
			return 0;
		}
		value = GetMenuEnteredName(p_menu, NULL, 0, (LegoU16**) &widgetResult);
		if (value == 0) {
			return 0;
		}
		player = ~*(LegoU32*) (menuAddress + 0x218) & 1;
		argument = (LegoU32) temporaryName.SetWideText((const LegoU16*) widgetResult);
		GetGameControllerSettings()->SetPlayerName((const GameControllerName*) argument, player);
		if (*(LegoS32*) (sessionAddress + 4) == 0) {
			PushMenuManagerActionState(p_menu, NULL, 0x20, 0xffffffff, 0);
			return 0;
		}
	LAB_00421d26:
		BeginMenuTransition(p_menu, NULL, 0);
		*(LegoU32*) (menuAddress + 0x1d8) = 2;
		return result;
	case 0x20:
		if (selected == 0) {
			if ((*(LegoS32*) (sessionAddress + 8) == 4) && (*(LegoS32*) (sessionAddress + 0xc) == 0)) {
				source = *(LegoU32**) (secondaryAddress + 0x234);
				destination = (LegoU32*) (sessionAddress + (~*(LegoU32*) (menuAddress + 0x218) & 1) * 0x344 + 0x6c);
				for (value = 0xbd; value != 0; value = value + -1) {
					*destination = *source;
					source = source + 1;
					destination = destination + 1;
				}
				PushMenuManagerActionState(p_menu, NULL, 0x22, 0xffffffff, 0);
				return 1;
			}
			PushMenuManagerActionState(p_menu, NULL, 0x21, 0xffffffff, 0);
			return 1;
		}
		break;
	case 0x21:
		if (selected == 0) {
			argument = 2;
			if (*(LegoS32*) (sessionAddress + 8) != 2) {
				PushMenuManagerActionState(p_menu, NULL, 0x22, 0xffffffff, 0);
				return 1;
			}
			if ((*(LegoS32*) sessionAddress != 1) || (*(LegoS32*) (menuAddress + 0x218) != 1)) {
				*(LegoU32*) (menuAddress + 0x21c) = *(LegoU32*) (menuAddress + 0x218) | 1;
				PushMenuManagerActionState(p_menu, NULL, 0x1a, 0xffffffff, 0);
				return 1;
			}
			value = IsMenuPlayerAvailable(2);
			goto joined_r0x00421e06;
		}
		break;
	case 0x22:
		if (selected == 0) {
			value = GetMenuManagerInitialValue(p_menu, NULL, 0);
			if (value == 0) {
				if ((*(LegoS32*) (sessionAddress + 8) != 2) &&
					((*(LegoS32*) sessionAddress != 2 || (*(LegoS32*) (networkAddress + 0x5c) != 2)))) {
					RestoreMenuManagerAction(p_menu, NULL, 0, 0x1b);
					return 1;
				}
				RestoreMenuManagerAction(p_menu, NULL, 0, 0x1f);
				return 1;
			}
			accept = true;
			if ((*(LegoS32*) sessionAddress == 1) && (*(LegoS32*) (menuAddress + 0x218) == 1)) {
				accept = false;
				value = IsMenuPlayerAvailable(2);
				if (value == 0) {
					result = 0;
					PlayMenuInteractionSound(2);
				}
				else {
					*(LegoU32*) (menuAddress + 0x21c) = 2;
					PushMenuManagerActionState(p_menu, NULL, 0x1f, 0xffffffff, 0);
				}
			}
			if (*(LegoS32*) sessionAddress == 2) {
				if (*(LegoS32*) (networkAddress + 0x5c) != 1) {
					PushMenuManagerActionState(p_menu, NULL, 0x16, 0xffffffff, 0);
					return result;
				}
				value = HostMenuNetworkSession(g_gameController.m_networkApp, NULL);
				if (value == 0) {
					return result;
				}
				PushMenuManagerActionState(p_menu, NULL, 0x17, 0xffffffff, 0);
				return result;
			}
			if (!accept) {
				return result;
			}
			value = IsMenuPlayerAvailable(*(LegoU32*) (menuAddress + 0x218) | 1);
			if (value != 0) {
				*(LegoU32*) (menuAddress + 0x218) = *(LegoU32*) (menuAddress + 0x218) | 1;
				BeginMenuTransition(p_menu, NULL, 1);
				*(LegoU32*) (menuAddress + 0x1d8) = 1;
				return result;
			}
			goto LAB_004211ce;
		}
		break;
	case 0x23:
		switch (selected) {
		case 0:
			argument = *(LegoU32*) (menuAddress + 0x218);
		LAB_00422077:
			value = IsMenuPlayerAvailable(argument);
			if (value != 0) {
			LAB_00422086:
				PopSecondaryMenuActions(p_menu, NULL, 1, 1);
				*(LegoU32*) (menuAddress + 0x1d8) = 7;
				return 1;
			}
			goto LAB_004211ce;
		case 1:
			PushMenuManagerActionState(p_menu, NULL, 0x25, 0xffffffff, 0);
			return 1;
		case 2:
		switchD_00422069_caseD_1:
			PushMenuManagerActionState(p_menu, NULL, 0x26, 0xffffffff, 0);
			return 1;
		case 3:
		switchD_00422069_caseD_2:
			PushMenuManagerActionState(p_menu, NULL, 0x27, 0xffffffff, 0);
			return 1;
		case 4:
			value = *(LegoS32*) (sessionAddress + 4);
		LAB_00422014:
			if (value != 0) {
				*(LegoU32*) (menuAddress + 0x204) = 4;
				PushMenuManagerActionState(p_menu, NULL, 0x2a, 0xffffffff, 0);
				return 1;
			}
		LAB_0042203b:
			*(LegoU32*) (menuAddress + 0x204) = 3;
			PushMenuManagerActionState(p_menu, NULL, 0x2a, 0xffffffff, 0);
			return 1;
		case 5:
		switchD_00422069_caseD_5:
			PushMenuManagerActionState(p_menu, NULL, 0x28, 0xffffffff, 0);
			return 1;
		}
		break;
	case 0x24:
		switch (selected) {
		case 0:
			argument = *(LegoU32*) (menuAddress + 0x218);
			goto LAB_00422077;
		case 1:
			goto switchD_00422069_caseD_1;
		case 2:
			goto switchD_00422069_caseD_2;
		case 3:
			value = *(LegoS32*) (sessionAddress + 4);
			goto LAB_00422014;
		case 5:
			goto switchD_00422069_caseD_5;
		}
		break;
	case 0x25:
		if (selected == 0) {
			value = GetMenuManagerInitialValue(p_menu, NULL, 0);
			if (value != 0) {
				value = IsMenuPlayerAvailable(*(LegoU32*) (menuAddress + 0x218));
				if (value != 0) {
					BeginMenuTransition(p_menu, NULL, 1);
					*(LegoU32*) (menuAddress + 0x1d8) = 8;
					return 1;
				}
				goto LAB_004211ce;
			}
			goto switchD_004213b8_caseD_5;
		}
		break;
	case 0x28:
		if (selected == 0) {
			value = GetMenuManagerInitialValue(p_menu, NULL, 0);
			if (value != 0) {
			LAB_004222b4:
				BeginMenuTransition(p_menu, NULL, 1);
				*(LegoU32*) (menuAddress + 0x1d8) = 0xd;
				return 1;
			}
			goto LAB_004211ff;
		}
		break;
	case 0x29:
		switch (selected) {
		case 0:
			value = IsMenuPlayerAvailable(*(LegoU32*) (menuAddress + 0x218));
			if (value != 0) {
				PopSecondaryMenuActions(p_menu, NULL, 1, 1);
				*(LegoU32*) (menuAddress + 0x1d8) = 9;
				return 1;
			}
			break;
		case 1:
			value = IsMenuPlayerAvailable(*(LegoU32*) (menuAddress + 0x218));
			if (value != 0) {
				PopSecondaryMenuActions(p_menu, NULL, 1, 1);
				*(LegoU32*) (menuAddress + 0x1d8) = 8;
				return 1;
			}
			break;
		case 2:
			PopSecondaryMenuActions(p_menu, NULL, 1, 1);
			*(LegoU32*) (menuAddress + 0x1d8) = 0xc;
			return 1;
		case 3:
		switchD_004221f4_caseD_3:
			PopSecondaryMenuActions(p_menu, NULL, 1, 1);
			*(LegoU32*) (menuAddress + 0x1d8) = 10;
			return 1;
		default:
			goto switchD_00420d7d_caseD_2c;
		case 5:
			if (*(LegoS32*) (sessionAddress + 4) == 0) {
				goto LAB_004222b4;
			}
			goto switchD_00422069_caseD_5;
		}
	LAB_004211ce:
		PlayMenuInteractionSound(2);
		return 0;
	case 0x2a:
		if (selected == 0) {
			value = GetMenuManagerInitialValue(p_menu, NULL, 0);
			if (value != 0) {
				if ((*(LegoS32*) (menuAddress + 0x204) != 1) && (*(LegoS32*) (menuAddress + 0x204) != 2)) {
					BeginSecondarySave(g_gameController.m_secondaryManager, NULL);
					return 1;
				}
				goto LAB_004221ba;
			}
			goto switchD_004213b8_caseD_5;
		}
		break;
	case 0x2b:
		if (selected == 0) {
			value = GetMenuManagerInitialValue(p_menu, NULL, 0);
			if (value == 0) {
				ResetSecondaryMenuAction(p_menu, NULL, 1);
				return 1;
			}
			*(LegoU32*) (menuAddress + 0x204) = 1;
		LAB_004221ba:
			BeginSecondaryLoad(g_gameController.m_secondaryManager, NULL);
			return 1;
		}
		break;
	case 0x2c:
		break;
	case 0x2d:
		if (selected == 0) {
			goto LAB_00422086;
		}
		if (selected == 1) {
			*(LegoU32*) (menuAddress + 0x204) = 4;
			PushMenuManagerActionState(p_menu, NULL, 0x2a, 0xffffffff, 0);
			return 1;
		}
		if (selected == 2) {
			goto switchD_004221f4_caseD_3;
		}
		break;
	case 0x2e:
		if (selected == 0) {
			value = GetMenuManagerInitialValue(p_menu, NULL, 0);
			if (value != 0) {
				*(LegoU32*) (menuAddress + 0x204) = 4;
				BeginSecondarySave(g_gameController.m_secondaryManager, NULL);
				return 1;
			}
		LAB_004223a5:
			BeginMenuTransition(p_menu, NULL, 1);
			*(LegoU32*) (menuAddress + 0x1d8) = 4;
			return 1;
		}
		break;
	case 0x2f:
		if ((-1 < selected) && (selected < 4)) {
			if (selected == 1) {
				argument = 0xb;
			}
			else if (selected == 2) {
				argument = 9;
			}
			else if (selected == 3) {
				argument = 0xc;
			}
			else {
				argument = 10;
			}
			SetMenuExitAction(g_gameController.m_unk0x8c, NULL, argument);
			goto LAB_004223a5;
		}
		break;
	case 0x30:
		if ((-1 < selected) && (selected < 3)) {
			IncrementMenuModeProgress(g_gameController.m_unk0x8c, NULL, selected);
			if (*(LegoS32*) (sessionAddress + 0x28) != 0) {
				SetControllerMenuMode((void*) *(LegoU32*) (sessionAddress + 0x28), NULL, 5);
			}
			BeginMenuTransition(p_menu, NULL, 1);
			*(LegoU32*) (menuAddress + 0x1d8) = 0xb;
			return 0;
		}
		break;
	default:
		goto switchD_00420d7d_default;
	}
switchD_00420d7d_caseD_2c:
	result = 0;
switchD_00420d7d_default:
	return result;
}

// STUB: LEGORACERS2 0x004231d0
LegoS32 __fastcall GetMenuParentAction(MenuManager* p_menu, void*)
{
	LegoS32 depth = p_menu->m_unk0x198;
	if (depth < 2) {
		return -1;
	}
	LegoU8* stack = (LegoU8*) p_menu + depth * 0x14;
	if (*(LegoU32*) (stack + 0x34) != *(LegoU32*) (stack + 0x48)) {
		return -1;
	}
	return *(LegoS32*) (stack + 0x30);
}

// STUB: LEGORACERS2 0x00423230
LegoS32 __fastcall GetSecondaryPendingMenuAction(MenuManager* p_menu, void*)
{
	LegoS32 depth = p_menu->m_unk0x198;
	while (depth > 0) {
		--depth;
		if (*(LegoU32*) ((LegoU8*) p_menu + depth * 0x14 + 0x60) != 0) {
			return *(LegoS32*) ((LegoU8*) p_menu + depth * 0x14 + 0x58);
		}
	}
	return -1;
}

// STUB: LEGORACERS2 0x00423940
LegoBool32 __fastcall GetMenuEnteredName(MenuManager* p_menu, void*, LegoS32 p_id, LegoU16** p_text)
{
	MenuTextEntry* widget = (MenuTextEntry*) p_menu->m_widgets[12];
	if (widget->m_initialValue != p_id) {
		return FALSE;
	}
	*p_text = widget->GetText();
	return *p_text != NULL;
}

// STUB: LEGORACERS2 0x00423980
LegoBool32 __fastcall CommitMenuInputBindings(MenuManager* p_menu, void*, LegoS32 p_id, LegoU32* p_bindings)
{
	MenuInputBindingWidget* widget = (MenuInputBindingWidget*) p_menu->m_widgets[13];
	if (widget->m_initialValue != p_id) {
		return TRUE;
	}
	if (widget->m_selectedAction != 14) {
		return FALSE;
	}
	memcpy(p_bindings, widget->m_bindingData, sizeof(widget->m_bindingData));
	return TRUE;
}
