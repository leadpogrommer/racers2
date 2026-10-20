#include "secondarymenubuilder.h"

#include "application.h"
#include "bitmapfontset.h"
#include "bodypartmanager.h"
#include "charbuffer24.h"
#include "controllerrecordingservice.h"
#include "displayconfig.h"
#include "gamecontroller.h"
#include "gamecontrollerconfig.h"
#include "gamecontrollersession.h"
#include "globaloverlaymanager.h"
#include "inputcontrollermanager.h"
#include "menucarpreviewselector.h"
#include "menucharacterpreviewselector.h"
#include "menuinputbindings.h"
#include "menuintegerselector.h"
#include "menulabelselector.h"
#include "menumanagercurrentaction.h"
#include "menunetworkstatus.h"
#include "menurangeslider.h"
#include "menutextentry.h"
#include "menutextgroup.h"
#include "menutoggle.h"
#include "menutrackselector.h"
#include "menuvolumeslider.h"
#include "menuwidgetinitialization.h"
#include "secondarymanager.h"
#include "secondarymenuactions.h"
#include "secondarymenuinteraction.h"
#include "simulationscheduler.h"

#include <new.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

template <class T>
static T& MenuActionField(MenuManager* p_menu, LegoU32 p_offset)
{
	return *(T*) ((LegoU8*) p_menu + p_offset);
}
static GlobalOverlayRecord* MenuActionRecord(MenuManager* p_menu, LegoU32 p_index)
{
	return (GlobalOverlayRecord*) p_menu->m_overlayRecords[p_index];
}

// GLOBAL: LEGORACERS2 0x00561750
static LegoFloat g_menuActionDefaultPosition[2];
// GLOBAL: LEGORACERS2 0x00561708
static LegoFloat g_menuActionPausePosition[2];
// GLOBAL: LEGORACERS2 0x0056123c
static LegoFloat g_menuActionHeaderOffset;
// GLOBAL: LEGORACERS2 0x00561728
static LegoFloat g_menuActionFooterPosition[2];
// GLOBAL: LEGORACERS2 0x00561748
static LegoU32 g_menuActionHeaderColors[2];
// GLOBAL: LEGORACERS2 0x00561720
static LegoFloat g_menuActionNoticePosition[2];
extern LegoU32 g_menuDisabledColor;
static void InitializeMenuActionDefaultPosition();
static void InitializeMenuActionPausePosition();
static void InitializeMenuActionHeaderOffset();
static void InitializeMenuActionFooterPosition();
static void InitializeMenuActionHeaderColors();
static void InitializeMenuActionNoticePosition();
struct MenuActionTuningStartup {
	MenuActionTuningStartup()
	{
		InitializeMenuActionDefaultPosition();
		InitializeMenuActionPausePosition();
		InitializeMenuActionHeaderOffset();
		InitializeMenuActionFooterPosition();
		InitializeMenuActionHeaderColors();
		InitializeMenuActionNoticePosition();
	}
};
static MenuActionTuningStartup g_menuActionTuningStartup;

void __fastcall EnableSecondaryInactivityTimer(void*, void*);
LegoBool32 __fastcall IsSecondaryCarUnlockFlagSet(const LegoU32*, void*, LegoU32);
LegoU32 __fastcall BeginMenuNetworkDiscovery(GameObject*, void*);
void __fastcall RefreshMenuDiscoveredNetworkSessions(GameObject*, void*);
void __fastcall BeginMenuNetworkHostSetup(GameObject*, void*);
static LegoBool32 __fastcall AreMenuControllersReady(InputControllerManager*, void*);
void __fastcall RefreshMenuNetworkList(MenuManager*, void*);
extern const LegoS32 g_secondaryWorldCarCounts[5];
extern const LegoS32 g_secondaryWorldCarStarts[5];
extern const LegoU32 g_secondarySessionRaceValues[26];
extern const LegoS32 g_secondarySpecialCarIndices[2];

// GLOBAL: LEGORACERS2 0x005302b0
static const LegoS32 g_menuLanguageChoices[8][2] =
	{{0, 576}, {1, 578}, {2, 574}, {4, 579}, {5, 581}, {6, 580}, {7, 582}, {8, 577}};
// GLOBAL: LEGORACERS2 0x005302f0
static const LegoS32 g_menuRestrictedLanguageChoices[1][2] = {{3, 574}};
// GLOBAL: LEGORACERS2 0x00561248
static LegoU32 g_menuResolutionNamesInitialized;
// GLOBAL: LEGORACERS2 0x00561250
static LegoU8 g_menuResolutionNameStorage[50 * sizeof(CharBuffer24)];
static void ReleaseMenuResolutionNames();
static void SetMenuActionText(MenuManager* p_menu, LegoS32 p_record, LegoS32 p_string)
{
	MenuActionRecord(p_menu, p_record)->InitializeTextByIndex(p_string, 0, 0.0f);
}
static void SetMenuActionCenter(MenuManager* p_menu, LegoFloat p_x, LegoFloat p_y)
{
	MenuActionField<LegoFloat>(p_menu, 0x1b0) = p_x;
	MenuActionField<LegoFloat>(p_menu, 0x1b4) = p_y;
}
static void ReadMenuActionCenter(MenuManager* p_menu, LegoFloat* p_position)
{
	p_position[0] = MenuActionField<LegoFloat>(p_menu, 0x1b0);
	p_position[1] = MenuActionField<LegoFloat>(p_menu, 0x1b4);
}
static void SetMenuActionTextList(MenuManager* p_menu, const LegoS32* p_pairs, LegoS32 p_count)
{
	for (LegoS32 index = 0; index < p_count; ++index) {
		SetMenuActionText(p_menu, p_pairs[index * 2], p_pairs[index * 2 + 1]);
	}
}
static void InitializeMenuActionChoice(
	MenuManager* p_menu,
	MenuWidgetBase* p_widget,
	const LegoFloat* p_position,
	LegoS32 p_first,
	LegoS32 p_second,
	LegoS32 p_selection = 0
)
{
	InitializeMenuChoiceSelectorFromMode(
		(MenuChoiceSelector*) p_widget,
		NULL,
		0,
		p_position,
		p_selection,
		-1,
		-1,
		p_first,
		p_second,
		0.8f,
		0,
		1
	);
}

// STUB: LEGORACERS2 0x00406d90
void __fastcall EnableSecondaryInactivityTimer(void* p_object, void*)
{
	LegoU8* timer = (LegoU8*) p_object;
	*(LegoFloat*) (timer + 0x60) = 45.0f;
	*(LegoU32*) (timer + 0x58) = 1;
	GlobalOverlayRecord** record = (GlobalOverlayRecord**) (timer + 0x64);
	if (*record != NULL) {
		(*record)->Release();
		*record = NULL;
	}
}

// STUB: LEGORACERS2 0x0040b390
LegoBool32 __fastcall IsSecondaryCarUnlockFlagSet(const LegoU32* p_flags, void*, LegoU32 p_car)
{
	if (p_car == 15) {
		return (*p_flags & 2) != 0;
	}
	if (p_car == 37) {
		return (*p_flags & 1) != 0;
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x0041c110
static void InitializeMenuActionDefaultPosition()
{
	g_menuActionDefaultPosition[0] = 0.7f;
	g_menuActionDefaultPosition[1] = 0.5f;
}
// STUB: LEGORACERS2 0x0041c140
static void InitializeMenuActionPausePosition()
{
	g_menuActionPausePosition[0] = 0.5f;
	g_menuActionPausePosition[1] = 0.45f;
}
// STUB: LEGORACERS2 0x0041c170
static void InitializeMenuActionHeaderOffset()
{
	g_menuActionHeaderOffset = -(0.06f * 2.5f + 0.06f * 0.5f);
}
// STUB: LEGORACERS2 0x0041c1b0
static void InitializeMenuActionFooterPosition()
{
	g_menuActionFooterPosition[0] = 0.5f;
	g_menuActionFooterPosition[1] = 0.95f;
}
// STUB: LEGORACERS2 0x0041c2a0
static void InitializeMenuActionHeaderColors()
{
	g_menuActionHeaderColors[0] = 0xffffffff;
	g_menuActionHeaderColors[1] = 0xff1478d2;
}
// STUB: LEGORACERS2 0x0041c2f0
static void InitializeMenuActionNoticePosition()
{
	g_menuActionNoticePosition[0] = 0.3f;
	g_menuActionNoticePosition[1] = 0.5f;
}

// STUB: LEGORACERS2 0x0041dec0
void __fastcall BuildMenuManagerAction(MenuManager* p_menu, void*, LegoU32 p_action, LegoS32 p_selected)
{
	LegoS32 headerColor = -1;
	p_menu->m_unk0x218 = MenuActionField<LegoU32>(p_menu, 0x54 + p_menu->m_unk0x198 * 0x14);
	SetMenuActionCenter(p_menu, g_menuActionDefaultPosition[0], g_menuActionDefaultPosition[1]);
	if (p_action >= 0x23 && p_action <= 0x28) {
		SetMenuActionCenter(p_menu, g_menuActionPausePosition[0], g_menuActionPausePosition[1]);
	}
	LegoFloat headerOffset = g_menuActionHeaderOffset;
	LegoFloat rowOffset = 0.06f, rowSpacing = 0.06f, secondaryOffset = 0.1f;
	LegoFloat headerScale = 0.5f, secondaryScale = 0.7f, footerScale = 0.7f;
	LegoFloat position[2];
	p_menu->m_unk0x228 = -1;
	p_menu->m_unk0x1dc = 0;
	p_menu->m_unk0x1e8 = 1;
	p_menu->m_unk0x1ec = 0;
	p_menu->m_unk0x1e0 = 0;
	for (LegoS32 disabled = 0; disabled < 5; ++disabled) {
		MenuActionField<LegoU32>(p_menu, 0x1f0 + disabled * 4) = 0;
	}
	MenuActionField<LegoU32>(p_menu, 0x1e4) = 1;
	switch (p_action) {
	case 0: {
		SetMenuActionCenter(p_menu, 0.5f, 0.5f);
		const LegoS32 text[] = {0, 0x1de, 1, 0x1df, 2, 5, 3, 0x1e0};
		SetMenuActionTextList(p_menu, text, 4);
		g_gameControllerSession->m_unk0x04 = 0;
		g_gameControllerSession->m_unk0x00 = -1;
		g_gameControllerSession->m_mode = -1;
		SetMenuActionInputMask(p_menu, NULL, 3);
		break;
	}
	case 1: {
		p_menu->m_unk0x1ec = 1;
		headerColor = 0;
		const LegoS32 text[] = {6, 0x8c, 0, 100, 1, 0x65, 2, 0x11, 3, 0xf, 4, 0x66, 5, 0x34b, 8, 0xa8};
		SetMenuActionTextList(p_menu, text, 8);
		g_gameControllerSession->m_unk0x04 = 0;
		g_gameControllerSession->m_unk0x00 = -1;
		g_gameControllerSession->m_mode = -1;
		SetMenuActionInputMask(p_menu, NULL, 3);
		EnableSecondaryInactivityTimer(g_gameController.m_unk0xc8, NULL);
		break;
	}
	case 2:
	case 0x13:
	case 0x15: {
		p_menu->m_unk0x1ec = 1;
		headerColor = 0;
		if (p_action == 2) {
			const LegoS32 text[] = {6, 100, 0, 0x67, 1, 0x68, 2, 0x69, 3, 0x6a, 4, 0x141, 5, 6, 8, 0xa8};
			SetMenuActionTextList(p_menu, text, 8);
		}
		else {
			const LegoS32 text[] = {6, p_action == 0x13 ? 0x75 : 0xb4, 0, 0x68, 1, 0x78, 2, 0x79, 5, 6, 8, 0xa8};
			SetMenuActionTextList(p_menu, text, 6);
		}
		if (p_action != 0x15) {
			g_gameControllerSession->m_unk0x04 = 0;
		}
		g_gameControllerSession->m_unk0x00 = p_action == 2 ? 0 : (p_action == 0x13 ? 1 : 2);
		g_gameControllerSession->m_mode = -1;
		if (p_action != 0x15) {
			SetMenuActionInputMask(p_menu, NULL, p_action == 2 ? 1 : 3);
		}
		break;
	}
	case 3: {
		SetMenuActionCenter(p_menu, 0.5f, 0.5f);
		headerColor = 0;
		const LegoS32 text[] = {6, 0x65, 0, 0x75, 1, 0x77, 5, 6, 8, 0xa8};
		SetMenuActionTextList(p_menu, text, 5);
		if (*(LegoU32*) ((LegoU8*) g_gameController.m_networkApp + 0x58) != 7) {
			MenuActionField<LegoU32>(p_menu, 0x1f4) = 1;
		}
		g_gameControllerSession->m_unk0x00 = -1;
		g_gameControllerSession->m_mode = -1;
		SetMenuActionInputMask(p_menu, NULL, 3);
		break;
	}
	case 4:
	case 5:
	case 6:
	case 0x10: {
		SetMenuActionCenter(p_menu, 0.5f, 0.5f);
		headerColor = 0;
		if (p_action == 4) {
			const LegoS32 text[] = {6, 0x11, 0, 0x12, 1, 0x13};
			SetMenuActionTextList(p_menu, text, 3);
		}
		else if (p_action == 5) {
			const LegoS32 text[] = {6, 0xf, 0, 0x7b, 1, 0x7c, 2, 0x7a};
			SetMenuActionTextList(p_menu, text, 4);
			Application* app = GetApplication();
			if (app->m_vtable->m_value0x7c(app, NULL) == 0) {
				SetMenuActionText(p_menu, 3, 0x23d);
			}
			SetMenuActionText(p_menu, 4, 10);
		}
		else if (p_action == 6) {
			const LegoS32 text[] = {6, 0x66, 0, 0x83, 1, 0x19};
			SetMenuActionTextList(p_menu, text, 3);
		}
		else {
			const LegoS32 text[] = {6, 0xaa, 0, 0x7d, 1, 0xab};
			SetMenuActionTextList(p_menu, text, 3);
		}
		SetMenuActionText(p_menu, 5, 6);
		SetMenuActionText(p_menu, 8, 0xa8);
		break;
	}
	case 7:
	case 9:
	case 0x18: {
		SetMenuActionCenter(p_menu, 0.5f, 0.5f);
		if (p_action == 7) {
			headerColor = 0;
		}
		else if (p_action == 9) {
			headerColor = 0;
		}
		LegoU32 title = p_action == 9 ? 0x7c : 0x11;
		if (p_action == 7) {
			title = p_menu->m_unk0x204 == 1 || p_menu->m_unk0x204 == 2 ? 0x83 : 0x19;
		}
		SetMenuActionText(p_menu, 6, title);
		SetMenuActionText(p_menu, 0, p_action == 7 ? 0x31e : 0xa9);
		MenuActionRecord(p_menu, 0)->AppendNarrowText("1", 1);
		SetMenuActionText(p_menu, 1, p_action == 7 ? 0x31e : 0xa9);
		MenuActionRecord(p_menu, 1)->AppendNarrowText("2", 1);
		SetMenuActionText(p_menu, 5, 6);
		if (p_action == 7) {
			LegoS32 parent = GetMenuParentAction(p_menu, NULL);
			LegoS32 record = 8;
			if (parent != 6 && GetMenuParentAction(p_menu, NULL) != 0x19) {
				record = 7;
			}
			SetMenuActionText(p_menu, record, 0xa8);
			p_selected = ((SecondaryManager*) g_gameController.m_secondaryManager)->m_unk0x240 == 2;
		}
		else {
			SetMenuActionText(p_menu, 8, 0xa8);
			SetMenuActionInputMask(p_menu, NULL, 3);
		}
		break;
	}
	case 8: {
		SetMenuActionCenter(p_menu, 0.5f, 0.5f);
		headerColor = 0;
		const LegoS32 text[] = {6, 0x7b, 0, 9, 1, 8, 2, 0x20c, 5, 6, 8, 0xa8};
		SetMenuActionTextList(p_menu, text, 6);
		break;
	}
	case 10: {
		SetMenuActionCenter(p_menu, 0.375f, 0.4f);
		rowSpacing = 0.12f;
		headerColor = 0;
		const LegoS32 text[] = {6, 0x7a, 0, 0x32c, 1, 0x32d, 2, 0x33f, 3, 0x80, 5, 6, 8, 0xa8};
		SetMenuActionTextList(p_menu, text, 7);
		ReadMenuActionCenter(p_menu, position);
		position[0] += (MeasureMenuActionWidth(p_menu, NULL, headerScale, 1.0f) * 0.5f - 0.08f);
		position[1] += rowOffset + headerOffset + 0.06f;
		for (LegoS32 setting = 0; setting < 3; ++setting) {
			MenuRangeSlider* slider = (MenuRangeSlider*) GetInactiveMenuWidget(p_menu, NULL, 3);
			if (slider != NULL) {
				LegoS32 value = *(LegoS32*) ((LegoU8*) g_gameControllerSettings + 0x20c + setting * 4) - 1;
				InitializeMenuRangeControl(
					slider,
					NULL,
					setting,
					position,
					value * (setting == 2 ? 0.25f : 0.1111111119f),
					setting == 2 ? 0.25f : 0.1111111119f,
					NULL,
					setting == 0 ? 0x398 : 0x20d,
					setting == 0 ? 0x399 : 0x20f,
					0.3f
				);
			}
			position[1] += 0.12f;
		}
		MenuRangeSlider* slider = (MenuRangeSlider*) GetInactiveMenuWidget(p_menu, NULL, 3);
		if (slider == NULL) {
			break;
		}
		if ((g_menuResolutionNamesInitialized & 1) == 0) {
			g_menuResolutionNamesInitialized |= 1;
			for (LegoS32 name = 0; name < 50; ++name) {
				new (g_menuResolutionNameStorage + name * sizeof(CharBuffer24)) CharBuffer24;
			}
			atexit(&ReleaseMenuResolutionNames);
		}
		SecondaryManager* secondary = (SecondaryManager*) g_gameController.m_secondaryManager;
		LegoS32 count = secondary->m_unk0x4bc;
		if (count > 1) {
			CharBuffer24* names = (CharBuffer24*) g_menuResolutionNameStorage;
			for (LegoS32 mode = 0; mode < count; ++mode) {
				sprintf(
					names[mode].m_data,
					"%d x %d x %d",
					secondary->m_displayModes[mode].m_width,
					secondary->m_displayModes[mode].m_height,
					secondary->m_displayModes[mode].m_depth
				);
			}
			LegoS32 selected = 0;
			for (; selected < count; ++selected) {
				const SecondaryDisplayMode& mode = secondary->m_displayModes[selected];
				if (g_displayConfiguration->m_width <= mode.m_width &&
					(g_displayConfiguration->m_width != mode.m_width ||
					 (g_displayConfiguration->m_height <= mode.m_height &&
					  (g_displayConfiguration->m_height != mode.m_height ||
					   g_displayConfiguration->m_bitDepth <= mode.m_depth)))) {
					break;
				}
			}
			if (selected >= count) {
				selected = count - 1;
			}
			LegoFloat step = 1.0f / (count - 1);
			InitializeMenuRangeControl(slider, NULL, 3, position, selected * step, step, names, 0x20d, 0x20f, 0.3f);
		}
		else {
			SetMenuActionText(p_menu, 8, -1);
		}
		break;
	}
	case 0xb: {
		SetMenuActionCenter(p_menu, 0.5f, 0.5f);
		SetMenuActionText(p_menu, 0, 0x23d);
		SetMenuActionText(p_menu, 8, GetSecondaryPendingMenuAction(p_menu, NULL) == 0xb ? 0xb7 : 0xb2);
		MenuLabelSelector* widget = (MenuLabelSelector*) GetInactiveMenuWidget(p_menu, NULL, 9);
		if (widget != NULL) {
			Application* app = GetApplication();
			LegoBool32 restricted = app->m_vtable->m_value0x7c(app, NULL) != 0;
			const LegoS32* choices = restricted ? &g_menuRestrictedLanguageChoices[0][0] : &g_menuLanguageChoices[0][0];
			app = GetApplication();
			LegoS32 count = app->m_vtable->m_value0x7c(app, NULL) != 0 ? 1 : 8;
			LegoS32 selected = 0;
			for (; selected < count; ++selected) {
				if (choices[selected * 2] == g_gameControllerSettings->m_language) {
					break;
				}
			}
			if (selected == count) {
				selected = 0;
			}
			LegoS32 labels[9];
			for (LegoS32 index = 0; index < count; ++index) {
				labels[index] = choices[index * 2 + 1];
			}
			LegoFloat center[2] = {0.5f, 0.44f};
			widget->Initialize(0, center, labels, count, selected, NULL, 1, 1.0f, -1.0f, 1);
		}
		break;
	}
	case 0xc:
	case 0x12: {
		SetMenuActionCenter(p_menu, 0.5f, 0.5f);
		SetMenuActionText(p_menu, 0, p_action == 0xc ? 10 : 0xab);
		SetMenuActionText(p_menu, 8, 0xb2);
		MenuToggle* widget = (MenuToggle*) GetInactiveMenuWidget(p_menu, NULL, 7);
		if (widget != NULL) {
			LegoFloat center[2] = {0.5f, 0.44f};
			LegoS32 value = *(LegoS32*) ((LegoU8*) g_gameControllerSettings +
										 (p_action == 0xc ? 0xa4 : 0xa8 + ((~p_menu->m_unk0x218 & 1) * 4)));
			InitializeMenuToggleControl(widget, NULL, 0, center, value, 1, 1);
		}
		break;
	}
	case 0xd:
	case 0xe: {
		SetMenuActionCenter(p_menu, 0.5f, 0.44f);
		p_menu->m_unk0x1dc = 1;
		SetMenuActionText(p_menu, 0, p_action == 0xd ? 9 : 8);
		SetMenuActionText(p_menu, 8, 0x1c);
		MenuVolumeSlider* widget = (MenuVolumeSlider*) GetInactiveMenuWidget(p_menu, NULL, 0);
		if (widget != NULL) {
			ReadMenuActionCenter(p_menu, position);
			position[1] += rowSpacing + rowOffset + headerOffset;
			InitializeMenuVolumeControl(
				widget,
				NULL,
				0,
				position,
				p_action == 0xd ? g_gameControllerSettings->m_effectGain : g_gameControllerSettings->m_musicGain,
				p_action == 0xd ? 2 : 1,
				0
			);
		}
		break;
	}
	case 0xf: {
		SetMenuActionCenter(p_menu, 0.5f, 0.5f);
		SetMenuActionText(p_menu, 0, 0x20c);
		SetMenuActionText(p_menu, 8, 0xb2);
		MenuLabelSelector* widget = (MenuLabelSelector*) GetInactiveMenuWidget(p_menu, NULL, 9);
		if (widget != NULL) {
			const LegoS32 choices[] = {0x20d, 0x20e, 0x20f};
			LegoFloat center[2] = {0.5f, 0.44f};
			widget
				->Initialize(0, center, choices, 3, g_gameControllerSettings->m_audioQuality, NULL, 1, 1.0f, -1.0f, 1);
		}
		break;
	}
	case 0x11: {
		SetMenuActionCenter(p_menu, 0.5f, 0.37f);
		p_menu->m_unk0x1e8 = 0;
		SetMenuActionText(p_menu, 0, 0x7d);
		MenuInputBindingWidget* widget = (MenuInputBindingWidget*) GetInactiveMenuWidget(p_menu, NULL, 13);
		if (widget != NULL) {
			ReadMenuActionCenter(p_menu, position);
			position[1] -= 0.11f;
			LegoU32 mode = ~p_menu->m_unk0x218 & 1;
			widget->Initialize(
				0,
				position,
				mode,
				(LegoU32*) ((LegoU8*) g_gameControllerSettings + mode * 0xac + 0xb4),
				(LegoU8*) g_gameControllerSettings + mode * -0xac + 0x160,
				1
			);
		}
		break;
	}
	case 0x14: {
		SetMenuActionCenter(p_menu, 0.5f, 0.5f);
		headerColor = 0;
		const LegoS32 text[] = {6, 0x77, 0, 0xb4, 1, 0xb3, 5, 6, 8, 0xa8};
		SetMenuActionTextList(p_menu, text, 5);
		g_gameControllerSession->m_unk0x00 = 2;
		g_gameControllerSession->m_mode = -1;
		SetMenuActionInputMask(p_menu, NULL, 1);
		BeginMenuNetworkHostSetup(g_gameController.m_networkApp, NULL);
		break;
	}
	case 0x16: {
		SetMenuActionCenter(p_menu, 0.5f, 0.5f);
		headerColor = 0;
		SetMenuActionText(p_menu, 6, 0xf0);
		static const LegoChar* const names[] = {"SESSION1", "SESSION2", "SESSION3", "SESSION4", "SESSION5"};
		for (LegoS32 index = 0; index < 5; ++index) {
			MenuActionRecord(p_menu, index)->InitializeNarrowText(names[index], 0, 0.0f);
		}
		SetMenuActionText(p_menu, 5, 6);
		SetMenuActionText(p_menu, 8, 0xa8);
		RefreshMenuNetworkList(p_menu, NULL);
		BeginMenuNetworkDiscovery(g_gameController.m_networkApp, NULL);
		break;
	}
	case 0x17: {
		SetMenuActionCenter(p_menu, 0.8f, 0.78f);
		p_menu->m_unk0x1e8 = 0;
		headerColor = 0;
		SetMenuActionText(p_menu, 6, 0xf1);
		if (*(LegoU32*) ((LegoU8*) g_gameController.m_networkApp + 0x5c) == 1) {
			SetMenuActionText(p_menu, 0, 0xf2);
			SetMenuActionText(p_menu, 1, 0xf3);
		}
		SetMenuActionText(p_menu, 2, 0xf4);
		SetMenuActionText(p_menu, 5, 6);
		SetMenuActionText(p_menu, 8, 0xa8);
		for (LegoU32 mode = 0; mode < 3; ++mode) {
			MenuNetworkStatusWidget* widget = (MenuNetworkStatusWidget*) GetInactiveMenuWidget(p_menu, NULL, 14);
			if (widget != NULL) {
				const LegoFloat centers[3][2] = {{0.68f, 0.07f}, {0.07f, 0.84f}, {0.07f, 0.22f}};
				widget->Initialize(-1, centers[mode], mode);
				widget->m_unk0x1c = 1;
				widget->m_unk0x24 = 1;
				if (mode == 0) {
					widget->m_unk0x20 = 1;
				}
			}
		}
		break;
	}
	case 0x19: {
		SetMenuActionCenter(p_menu, 0.5f, 0.5f);
		headerColor = 0;
		const LegoS32 text[] = {6, 0x67, 0, 0xac, 1, 0x340, 5, 6, 8, 0xa8};
		SetMenuActionTextList(p_menu, text, 5);
		if (*(LegoU32*) ((LegoU8*) g_gameController.m_unk0x8c + 0x1e34) != 1) {
			MenuActionField<LegoU32>(p_menu, 0x1f4) = 1;
		}
		break;
	}
	case 0x1a: {
		SetMenuActionCenter(p_menu, 0.32f, 0.5f);
		p_menu->m_unk0x1dc = 1;
		headerColor = 0;
		const LegoS32 text[] = {6, 0x78, 0, 0xf2, 1, 0x11a, 2, 0x11b, 3, 0x72, 4, 0x11c, 5, 6, 8, 0xa8};
		SetMenuActionTextList(p_menu, text, 8);
		LegoS32 count = *(LegoS32*) ((LegoU8*) g_gameControllerSession + 0x1a6c);
		if (count == 0) {
			MenuActionField<LegoU32>(p_menu, 0x1f0) = 1;
			MenuActionField<LegoU32>(p_menu, 0x1f8) = 1;
			MenuActionField<LegoU32>(p_menu, 0x1fc) = 1;
			p_selected = 1;
		}
		else if (count > 9) {
			MenuActionField<LegoU32>(p_menu, 0x1f4) = 1;
			MenuActionField<LegoU32>(p_menu, 0x200) = 1;
			p_selected = 0;
		}
		MenuTrackSelector* widget = (MenuTrackSelector*) GetInactiveMenuWidget(p_menu, NULL, 17);
		if (widget != NULL) {
			LegoFloat center[2] = {0.73f, 0.35f};
			widget->Initialize(-1, center, 1);
			widget->m_unk0x24 = 1;
		}
		break;
	}
	case 0x1b: {
		p_menu->m_unk0x1ec = 1;
		headerColor = 0;
		const LegoS32 text[] = {6, 0x6b, 0, 0x84, 1, 0x86, 2, 0x87, 3, 0x85, 4, 0x88, 5, 6, 8, 0xa8};
		SetMenuActionTextList(p_menu, text, 8);
		const LegoU32 order[5] = {0, 3, 1, 2, 4};
		SecondaryManagerMediaState* unlocks =
			(SecondaryManagerMediaState*) ((LegoU8*) g_gameController.m_unk0x8c + 0x1e04);
		for (LegoU32 world = 0; world < 5; ++world) {
			if (!unlocks->IsSlotEnabled(world)) {
				MenuActionField<LegoU32>(p_menu, 0x1f0 + order[world] * 4) = 1;
			}
		}
		LegoU32 lastWorld = *(LegoU32*) ((LegoU8*) g_gameControllerSettings + 0x7f9c);
		LegoU32 chosen = lastWorld >= 1 && lastWorld <= 4 ? order[lastWorld] : 0;
		if (MenuActionField<LegoU32>(p_menu, 0x1f0 + chosen * 4) == 0) {
			p_selected = chosen;
		}
		break;
	}
	case 0x1c: {
		SetMenuActionCenter(p_menu, 0.42f, 0.5f);
		p_menu->m_unk0x1dc = 1;
		headerColor = 0;
		LegoS32 world = g_gameControllerSession->m_selectedMode;
		if (g_gameControllerSession->m_mode == 2) {
			LegoS32 selected = *(LegoS32*) ((LegoU8*) g_gameControllerSession + 0x1a70);
			world = *(LegoS32*) ((LegoU8*) g_gameControllerSession + 0x1a78 + selected * 4);
		}
		LegoS32 first, count = 5;
		switch (world) {
		case 1:
			first = 141;
			break;
		case 2:
			first = 151;
			break;
		case 3:
			first = 156;
			break;
		case 4:
			first = 161;
			break;
		default:
			first = 146;
			count = 4;
			break;
		}
		SetMenuActionText(p_menu, 6, g_gameControllerSession->m_mode == -1 ? 0x69 : 0x6c);
		for (LegoS32 track = 0; track < count; ++track) {
			SetMenuActionText(p_menu, track, first + track);
		}
		SetMenuActionText(p_menu, 5, 6);
		SetMenuActionText(p_menu, 8, 0xa8);
		GameControllerSelection* selection = (GameControllerSelection*) ((LegoU8*) g_gameController.m_unk0x8c + 0x1e04);
		for (LegoS32 choice = 0; choice < count; ++choice) {
			if (!selection->IsTrackUnlocked(world, choice + 1)) {
				MenuActionField<LegoU32>(p_menu, 0x1f0 + choice * 4) = 1;
			}
		}
		LegoS32 remembered = *(LegoS32*) ((LegoU8*) g_gameControllerSettings + 0x7fa0 + world * 4);
		if (MenuActionField<LegoU32>(p_menu, 0x1ec + remembered * 4) == 0) {
			p_selected = remembered - 1;
		}
		break;
	}
	case 0x1d: {
		SetMenuActionCenter(p_menu, 0.5f, 0.5f);
		SetMenuActionText(p_menu, 0, 0xb);
		SetMenuActionText(p_menu, 8, 0xb2);
		MenuIntegerSelector* widget = (MenuIntegerSelector*) GetInactiveMenuWidget(p_menu, NULL, 8);
		if (widget != NULL) {
			LegoS32 world = g_gameControllerSession->m_selectedMode, track = g_gameControllerSession->m_track;
			if (g_gameControllerSession->m_mode == 2) {
				LegoS32 selected = *(LegoS32*) ((LegoU8*) g_gameControllerSession + 0x1a70);
				world = *(LegoS32*) ((LegoU8*) g_gameControllerSession + 0x1a78 + selected * 4);
				track = *(LegoS32*) ((LegoU8*) g_gameControllerSession + 0x1aa0 + selected * 4);
			}
			LegoFloat center[2] = {0.5f, 0.44f};
			LegoS32 value = *(LegoS32*) ((LegoU8*) g_gameControllerSettings + (track + (world + 2) * 5) * 4);
			InitializeMenuIntegerControl(widget, NULL, 0, center, value, 1, 5, 1);
		}
		break;
	}
	case 0x1e: {
		SetMenuActionCenter(p_menu, 0.5f, 0.5f);
		p_menu->m_unk0x1dc = 1;
		headerColor = 0;
		const LegoS32 text[] = {6, 0xb0, 0, 0xad, 1, 0xae, 5, 6, 8, 0xa8};
		SetMenuActionTextList(p_menu, text, 5);
		GameControllerSelection* selection = (GameControllerSelection*) ((LegoU8*) g_gameController.m_unk0x8c + 0x1e04);
		for (LegoU32 reward = 0; reward < 2; ++reward) {
			if (!selection->HasReward(g_gameControllerSession->m_selectedMode, reward)) {
				MenuActionField<LegoU32>(p_menu, 0x1f0 + reward * 4) = 1;
			}
		}
		break;
	}
	case 0x1f: {
		SetMenuActionCenter(p_menu, 0.5f, 0.36f);
		p_menu->m_unk0x1e8 = 0;
		SetMenuActionText(p_menu, 0, 0xb6);
		SetMenuActionText(p_menu, 8, 0xb1);
		MenuTextEntry* widget = (MenuTextEntry*) GetInactiveMenuWidget(p_menu, NULL, 12);
		if (widget != NULL) {
			LegoFloat center[2] = {0.5f, 0.25f};
			const LegoU16* name =
				(const LegoU16*) ((LegoU8*) g_gameControllerSettings + 4 + (~p_menu->m_unk0x218 & 1) * 20);
			widget->Initialize(0, center, name, 1);
		}
		break;
	}
	case 0x20: {
		SetMenuActionCenter(p_menu, 0.5f, 0.36f);
		SetMenuActionText(p_menu, 0, 0x6e);
		SetMenuActionText(p_menu, 8, 0xb2);
		MenuCharacterPreviewSelector* widget = (MenuCharacterPreviewSelector*) GetInactiveMenuWidget(p_menu, NULL, 10);
		if (widget != NULL) {
			LegoFloat center[2] = {0.5f, 0.3f};
			widget->Initialize(0, center, 1.0f, 0, 1, 1);
			widget->m_unk0x1c = 1;
			BodypartManager* parts = (BodypartManager*) g_gameController.m_bodypartManager;
			LegoS32 face = parts->m_faceTextureCount - 8;
			for (LegoS32 index = 0; index < 8; ++index) {
				MenuCharacterPreviewChoice choice;
				((LegoU32*) &choice)[0] = parts->m_headModelCount - 8 + index;
				((LegoU32*) &choice)[1] = face + index;
				((LegoU32*) &choice)[2] = parts->m_torsoTextureCount - 8 + index;
				((LegoU32*) &choice)[3] = parts->m_legTextureCount - 8 + index;
				widget->AddChoice(&choice);
			}
			for (LegoS32 saved = 0; saved < 2; ++saved) {
				widget->AddChoice((const MenuCharacterPreviewChoice*) ((LegoU8*) g_gameControllerSettings + 0x7f6c +
																	   saved * 16));
			}
			widget->SelectChoice(*(LegoS32*) ((LegoU8*) g_gameControllerSettings + 0x7f8c +
											  (~p_menu->m_unk0x218 & 1) * 4));
		}
		break;
	}
	case 0x21: {
		SetMenuActionCenter(p_menu, 0.5f, 0.36f);
		SetMenuActionText(p_menu, 0, 0x6d);
		SetMenuActionText(p_menu, 8, 0xb2);
		MenuCarPreviewSelector* widget = (MenuCarPreviewSelector*) GetInactiveMenuWidget(p_menu, NULL, 11);
		if (widget != NULL) {
			LegoS32 starts[5] = {0, 0, 0, 0, 0}, counts[5] = {0, 0, 0, 0, 0};
			LegoS32 count = 0;
			LegoBool32 includeSaved = TRUE;
			LegoU32 disabled = 0;
			if (g_gameControllerSession->m_mode == 4 && g_gameControllerSession->m_selectedMode == 0) {
				starts[0] = 45;
				counts[0] = 1;
				includeSaved = FALSE;
				disabled = 1;
			}
			else {
				const LegoS32 order[5] = {0, 2, 3, 1, 4};
				for (LegoS32 world = 0; world < 5; ++world) {
					starts[world] = g_secondaryWorldCarStarts[order[world]];
					counts[world] = g_secondaryWorldCarCounts[order[world]];
				}
			}
			LegoFloat center[2] = {0.5f, 0.3f};
			widget->Initialize(0, center, 1.0f, disabled, 1, 1);
			widget->m_unk0x1c = 1;
			SecondaryManager* secondary = (SecondaryManager*) g_gameController.m_secondaryManager;
			for (LegoS32 group = 0; group < 5; ++group) {
				if (counts[group] > 0) {
					count += counts[group];
					for (LegoS32 car = 0; car < counts[group]; ++car) {
						widget->AddChoice((LegoS32) secondary->m_secondaryData[starts[group] + car]);
					}
				}
			}
			if (includeSaved) {
				count += 40;
				for (LegoS32 saved = 0; saved < 40; ++saved) {
					widget->AddChoice((LegoS32) ((LegoU8*) g_gameControllerSettings + 0x94c + saved * 0x2f4));
				}
				for (LegoU32 special = 0; special < g_secondarySessionRaceValues[0]; ++special) {
					LegoS32 car = g_secondarySpecialCarIndices[special];
					if (IsSecondaryCarUnlockFlagSet(
							(const LegoU32*) ((LegoU8*) g_gameControllerSettings + 0x240),
							NULL,
							car
						)) {
						widget->AddChoice((LegoS32) secondary->m_secondaryData[car]);
						++count;
					}
				}
			}
			LegoS32 selected =
				*(LegoS32*) ((LegoU8*) g_gameControllerSettings + 0x7f94 + (~p_menu->m_unk0x218 & 1) * 4);
			if (selected >= count) {
				selected = 0;
			}
			widget->SelectChoice(selected);
		}
		break;
	}
	case 0x22: {
		SetMenuActionCenter(p_menu, 0.4f, 0.36f);
		if (g_gameControllerSession->m_unk0x00 != 2 ||
			*(LegoU32*) ((LegoU8*) g_gameController.m_networkApp + 0x5c) != 2) {
			p_menu->m_unk0x1dc = 1;
		}
		LegoU32 player = ~p_menu->m_unk0x218 & 1;
		MenuActionRecord(p_menu, 0)
			->InitializeText((const LegoU16*) ((LegoU8*) g_gameControllerSettings + 4 + player * 20), 0, 0.0f);
		SetMenuActionText(p_menu, 8, 0xb7);
		LegoU8* session = (LegoU8*) g_gameControllerSession + player * 0x344;
		MenuCharacterPreviewSelector* character =
			(MenuCharacterPreviewSelector*) GetInactiveMenuWidget(p_menu, NULL, 10);
		if (character != NULL) {
			ReadMenuActionCenter(p_menu, position);
			position[0] += 0.125f;
			position[1] += 0.09f;
			character->Initialize(-1, position, 0.5f, 1, 0, 1);
			character->AddChoice((const MenuCharacterPreviewChoice*) (session + 0x370));
			character->SelectChoice(0);
			character->m_unk0x1c = 1;
		}
		MenuCarPreviewSelector* car = (MenuCarPreviewSelector*) GetInactiveMenuWidget(p_menu, NULL, 11);
		if (car != NULL) {
			ReadMenuActionCenter(p_menu, position);
			position[0] -= 0.1f;
			position[1] -= 0.06f;
			car->Initialize(-1, position, 0.5f, 1, 1, 1);
			car->AddChoice((LegoS32) (session + 0x6c));
			car->SelectChoice(0);
			car->m_unk0x1c = 1;
		}
		MenuWidgetBase* choice = GetInactiveMenuWidget(p_menu, NULL, 1);
		if (choice != NULL) {
			ReadMenuActionCenter(p_menu, position);
			position[0] = 0.5f;
			position[1] += 0.44f;
			InitializeMenuActionChoice(p_menu, choice, position, 0xb8, -1, 1);
			choice->m_unk0x1c = 1;
			choice->m_unk0x24 = 1;
		}
		break;
	}
	case 0x23:
	case 0x24: {
		headerColor = 1;
		SetMenuActionText(p_menu, 6, 0x16);
		SetMenuActionText(p_menu, 0, 0x17);
		if (p_action == 0x23) {
			SetMenuActionText(p_menu, 1, 0x18);
			SetMenuActionText(p_menu, 2, 8);
			SetMenuActionText(p_menu, 3, 9);
			if (g_gameControllerSession->m_unk0x04 != 0 && g_gameControllerSession->m_mode == 7) {
				SetMenuActionText(p_menu, 4, 0x19);
			}
			LegoS32 label = 0x239;
			if (g_gameControllerSession->m_mode == 3) {
				label = 0x33c;
			}
			else if (g_gameControllerSession->m_mode == 4) {
				label = 0x33b;
			}
			else if (g_gameControllerSession->m_mode == 2) {
				label = 0x3a3;
			}
			SetMenuActionText(p_menu, 5, label);
		}
		else {
			SetMenuActionText(p_menu, 1, 8);
			SetMenuActionText(p_menu, 2, 9);
			SetMenuActionText(p_menu, 3, 0x19);
			SetMenuActionText(
				p_menu,
				5,
				*(LegoU32*) ((LegoU8*) g_gameController.m_foyerManager + 0xe4) != 0 ? 0x247 : 0x23a
			);
		}
		break;
	}
	case 0x25:
	case 0x26:
	case 0x27: {
		headerColor = 1;
		SetMenuActionText(p_menu, 6, 0x16);
		SetMenuActionText(p_menu, 0, p_action == 0x25 ? 0x18 : (p_action == 0x26 ? 8 : 9));
		SetMenuActionText(p_menu, 7, 0x1c);
		MenuWidgetBase* widget = GetInactiveMenuWidget(p_menu, NULL, p_action == 0x25 ? 1 : 0);
		if (widget != NULL) {
			ReadMenuActionCenter(p_menu, position);
			position[1] += rowSpacing + rowOffset + headerOffset + (p_action == 0x25 ? 0.04f : 0.0f);
			if (p_action == 0x25) {
				InitializeMenuActionChoice(p_menu, widget, position, 0x1b, -1);
			}
			else {
				InitializeMenuVolumeControl(
					(MenuVolumeSlider*) widget,
					NULL,
					0,
					position,
					p_action == 0x26 ? g_gameControllerSettings->m_musicGain : g_gameControllerSettings->m_effectGain,
					p_action == 0x26 ? 1 : 2,
					0
				);
			}
			secondaryOffset = (widget->m_bounds[3] - widget->m_bounds[1]) + (p_action == 0x25 ? 0.08f : 0.03f);
		}
		break;
	}
	case 0x28: {
		LegoS32 label = 0x239;
		if (g_unk0x00679fe0 == 0) {
			if (g_gameControllerSession->m_unk0x04 != 0) {
				label = 0x23a;
			}
		}
		else {
			headerColor = 1;
			SetMenuActionText(p_menu, 6, 0x16);
			switch (g_gameControllerSession->m_mode) {
			case 2:
				label = 0x3a3;
				break;
			case 3:
				label = 0x33c;
				break;
			case 4:
				label = 0x33b;
				break;
			case 7:
				label = *(LegoU32*) ((LegoU8*) g_gameController.m_foyerManager + 0xe4) != 0 ? 0x247 : 0x23a;
				break;
			}
		}
		SetMenuActionText(p_menu, 0, label);
		SetMenuActionText(p_menu, 7, 0x1c);
		MenuWidgetBase* widget = GetInactiveMenuWidget(p_menu, NULL, 1);
		if (widget != NULL) {
			ReadMenuActionCenter(p_menu, position);
			position[1] += rowSpacing + rowOffset + headerOffset + 0.04f;
			InitializeMenuActionChoice(
				p_menu,
				widget,
				position,
				label == 0x23a ? 0x348 : 0x1b,
				label == 0x23a ? 0x1b : -1
			);
			secondaryOffset = (widget->m_bounds[3] - widget->m_bounds[1]) + 0.08f;
		}
		break;
	}
	case 0x29: {
		SetMenuActionCenter(p_menu, 0.5f, 0.5f);
		headerColor = 1;
		LegoS32 title =
			g_gameControllerSession->m_mode == 4 ? 0x39a : (g_gameControllerSession->m_mode == 3 ? 0x39b : 99);
		SetMenuActionText(p_menu, 6, title);
		if (g_gameControllerSession->m_unk0x04 != 0) {
			SetMenuActionText(p_menu, 0, 0x14c);
			if (g_gameControllerSession->m_selectedMode != 0) {
				SetMenuActionText(p_menu, 3, 0x23b);
			}
		}
		if (g_gameControllerSession->m_mode == 2) {
			if (*(LegoS32*) ((LegoU8*) g_gameControllerSession + 0x1a70) <
				*(LegoS32*) ((LegoU8*) g_gameControllerSession + 0x1a6c)) {
				SetMenuActionText(p_menu, 0, 0x13f);
			}
			MenuTextGroup* widget = (MenuTextGroup*) GetInactiveMenuWidget(p_menu, NULL, 2);
			if (widget != NULL) {
				ReadMenuActionCenter(p_menu, position);
				position[1] += rowSpacing * 3.0f + rowOffset + headerOffset + 0.1f;
				InitializeMenuSessionTrackGroup(widget, NULL, -1, position);
				widget->m_unk0x1c = 1;
				widget->m_unk0x24 = 1;
			}
		}
		else {
			SetMenuActionText(p_menu, 1, 0x238);
		}
		if (g_gameControllerSession->m_mode != 3 && g_gameControllerSession->m_mode != 4 &&
			g_gameControllerSession->m_unk0x00 != 2 && AreMenuControllersReady(g_inputControllerManager, NULL)) {
			SetMenuActionText(p_menu, 2, 0x62);
		}
		LegoS32 exit = 7;
		if (g_gameControllerSession->m_unk0x04 == 0) {
			if (g_gameControllerSession->m_mode == 4) {
				exit = 0x33b;
			}
		}
		else {
			exit = 0x23a;
		}
		SetMenuActionText(p_menu, 5, exit);
		break;
	}
	case 0x2a: {
		if (GetSecondaryPendingMenuAction(p_menu, NULL) == 0x2d) {
			SetMenuActionCenter(p_menu, g_menuActionNoticePosition[0], g_menuActionNoticePosition[1]);
		}
		else {
			SetMenuActionCenter(p_menu, 0.5f, 0.45f);
		}
		LegoS32 title = p_menu->m_unk0x204 == 2 ? 0x340 : (p_menu->m_unk0x204 == 1 ? 0x83 : 0x19);
		SetMenuActionText(p_menu, 0, title);
		SetMenuActionText(p_menu, g_gameController.m_state != 0 ? 7 : 8, 0x1c);
		MenuWidgetBase* widget = GetInactiveMenuWidget(p_menu, NULL, 1);
		if (widget != NULL) {
			ReadMenuActionCenter(p_menu, position);
			position[1] += rowSpacing + rowOffset + headerOffset + 0.06f;
			InitializeMenuActionChoice(p_menu, widget, position, 0x1b, -1);
			secondaryOffset = (widget->m_bounds[3] - widget->m_bounds[1]) + 0.08f;
		}
		break;
	}
	case 0x2b:
	case 0x2e: {
		SetMenuActionCenter(p_menu, 0.5f, 0.5f);
		SetMenuActionText(p_menu, 0, p_action == 0x2b ? 0x83 : 0x19);
		MenuWidgetBase* widget = GetInactiveMenuWidget(p_menu, NULL, 1);
		if (widget != NULL) {
			ReadMenuActionCenter(p_menu, position);
			position[1] += rowOffset + headerOffset + (p_action == 0x2b ? 0.08f : 0.06f);
			InitializeMenuActionChoice(p_menu, widget, position, -1, -1);
		}
		break;
	}
	case 0x2d: {
		SetMenuActionCenter(p_menu, g_menuActionNoticePosition[0], g_menuActionNoticePosition[1]);
		const LegoS32 text[] = {0, 0x17, 1, 0x19, 2, 0x23b};
		SetMenuActionTextList(p_menu, text, 3);
		break;
	}
	case 0x2f:
	case 0x30: {
		SetMenuActionCenter(p_menu, 0.5f, 0.5f);
		headerColor = 1;
		if (p_action == 0x2f) {
			const LegoS32 text[] = {6, 0x14e, 0, 0x86, 1, 0x87, 2, 0x85, 3, 0x88};
			SetMenuActionTextList(p_menu, text, 5);
		}
		else {
			const LegoS32 text[] = {6, 0x1da, 0, 0x1db, 1, 0x1dc, 2, 0x1dd};
			SetMenuActionTextList(p_menu, text, 4);
		}
		break;
	}
	}
	CountMenuActionOptions(p_menu, NULL);
	for (LegoS32 color = 0; color < 5; ++color) {
		LegoU32 white = 0xffffffff;
		MenuActionRecord(p_menu, color)
			->SetColor(MenuActionField<LegoU32>(p_menu, 0x1f0 + color * 4) == 0 ? &white : &g_menuDisabledColor);
	}
	LegoS32 selected = p_selected;
	if (p_menu->m_unk0x22c != 0) {
		while (!IsMenuOptionEnabled(p_menu, NULL, selected)) {
			++selected;
			if (selected > 5) {
				selected = 0;
			}
			if (selected == p_selected) {
				break;
			}
		}
	}
	MenuActionField<LegoS32>(p_menu, (p_menu->m_unk0x198 + 4) * 0x14) = selected;
	HighlightMenuOption(p_menu, NULL, selected, 1);
	ReadMenuActionCenter(p_menu, position);
	position[1] += headerOffset;
	MenuActionRecord(p_menu, 6)->SetScale(headerScale);
	MenuActionRecord(p_menu, 6)->SetPosition(position, 1);
	if (headerColor >= 0) {
		MenuActionRecord(p_menu, 6)->SetColor(&g_menuActionHeaderColors[headerColor]);
	}
	position[1] += rowOffset;
	for (LegoS32 row = 0; row < 5; ++row) {
		GlobalOverlayRecord* record = MenuActionRecord(p_menu, row);
		if (record->m_text[0] != 0) {
			record->SetPosition(position, 1);
			position[1] += rowSpacing;
		}
	}
	MenuActionRecord(p_menu, 5)->SetPosition(position, 1);
	position[1] += secondaryOffset;
	MenuActionRecord(p_menu, 7)->SetScale(secondaryScale);
	MenuActionRecord(p_menu, 7)->SetPosition(position, 1);
	MenuActionRecord(p_menu, 8)->SetScale(footerScale);
	MenuActionRecord(p_menu, 8)->SetPosition(g_menuActionFooterPosition, 1);
	UpdateMenuActionBounds(p_menu, NULL);
	ResetMenuSelectionWidgets(p_menu, NULL, 1);
}

// STUB: LEGORACERS2 0x00420bd0
static void ReleaseMenuResolutionNames()
{
}

// STUB: LEGORACERS2 0x00420c80
MenuWidgetBase* __fastcall GetInactiveMenuWidget(MenuManager* p_menu, void*, LegoU32 p_type)
{
	LegoS32 count;
	switch (p_type) {
	case 0:
	case 1:
	case 2:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 17:
		count = 1;
		break;
	case 3:
		count = 4;
		break;
	case 14:
		count = 3;
		break;
	default:
		return NULL;
	}
	LegoU32 last = p_type + count - 1;
	for (LegoU32 index = p_type; index <= last; ++index) {
		if (p_menu->m_widgets[index]->m_state == 0) {
			return p_menu->m_widgets[index];
		}
	}
	return NULL;
}

// STUB: LEGORACERS2 0x00422fd0
void __fastcall SetMenuActionInputMask(MenuManager* p_menu, void*, LegoU32 p_mask)
{
	MenuActionField<LegoU32>(p_menu, 0x54 + p_menu->m_unk0x198 * 0x14) = p_mask;
	p_menu->m_unk0x218 = p_mask;
}

// STUB: LEGORACERS2 0x00423060
void __fastcall CountMenuActionOptions(MenuManager* p_menu, void*)
{
	p_menu->m_unk0x22c = 0;
	for (LegoU32 index = 0; index < 6; ++index) {
		if (MenuActionRecord(p_menu, index)->m_text[0] != 0) {
			++p_menu->m_unk0x22c;
		}
	}
}

static void IncludeMenuActionRectangle(MenuManager* p_menu, const LegoFloat* p_minimum, const LegoFloat* p_maximum)
{
	if (p_minimum[0] < p_menu->m_panelTopLeft[0]) {
		p_menu->m_panelTopLeft[0] = p_minimum[0];
	}
	if (p_minimum[1] < p_menu->m_panelTopLeft[1]) {
		p_menu->m_panelTopLeft[1] = p_minimum[1];
	}
	if (p_menu->m_panelBottomRight[0] < p_maximum[0]) {
		p_menu->m_panelBottomRight[0] = p_maximum[0];
	}
	if (p_menu->m_panelBottomRight[1] < p_maximum[1]) {
		p_menu->m_panelBottomRight[1] = p_maximum[1];
	}
}

// STUB: LEGORACERS2 0x00423260
void __fastcall UpdateMenuActionBounds(MenuManager* p_menu, void*)
{
	LegoFloat minimum[2] = {0.0f, 0.0f};
	LegoFloat maximum[2];
	p_menu->m_panelTopLeft[0] = p_menu->m_panelTopLeft[1] = 1.0f;
	p_menu->m_panelBottomRight[0] = p_menu->m_panelBottomRight[1] = 0.0f;
	for (LegoS32 index = 0; index < 8; ++index) {
		GlobalOverlayRecord* record = MenuActionRecord(p_menu, index);
		if (record->m_text[0] != 0) {
			record->GetRectangle(minimum, maximum);
			IncludeMenuActionRectangle(p_menu, minimum, maximum);
		}
	}
	for (LegoS32 widgetIndex = 0; widgetIndex < 18; ++widgetIndex) {
		MenuWidgetBase* widget = p_menu->m_widgets[widgetIndex];
		if (widget != NULL && widget->m_state != 0 && widget->m_unk0x24 == 0) {
			IncludeMenuActionRectangle(p_menu, widget->m_bounds, widget->m_bounds + 2);
		}
	}
	if (p_menu->m_unk0x19c == 0x16) {
		LegoFloat left = MenuActionField<LegoFloat>(p_menu, 0x1b0) - 0.25f;
		LegoFloat right = MenuActionField<LegoFloat>(p_menu, 0x1b0) + 0.25f;
		if (left >= 0.0f) {
			p_menu->m_panelTopLeft[0] = left;
		}
		if (right >= 0.0f) {
			p_menu->m_panelBottomRight[0] = right;
		}
	}
	else if (p_menu->m_unk0x19c == 0x17 && *(LegoU32*) ((LegoU8*) g_gameController.m_networkApp + 0x5c) == 1) {
		GlobalOverlayRecord* record = MenuActionRecord(p_menu, 1);
		record->InitializeTextByIndex(0x10b, 1, 0.0f);
		record->GetRectangle(minimum, maximum);
		IncludeMenuActionRectangle(p_menu, minimum, maximum);
		record->InitializeTextByIndex(0xf3, 1, 0.0f);
	}
}

// STUB: LEGORACERS2 0x00423540
LegoFloat __fastcall MeasureMenuActionWidth(
	MenuManager* p_menu,
	void*,
	LegoFloat p_headerScale,
	LegoFloat p_secondaryScale
)
{
	LegoFloat width = 0.0f;
	for (LegoS32 index = 0; index < 8; ++index) {
		GlobalOverlayRecord* record = MenuActionRecord(p_menu, index);
		if (record->m_text[0] != 0) {
			LegoFloat measured = record->m_measuredWidth;
			if (index == 6) {
				measured *= p_headerScale;
			}
			else if (index == 7) {
				measured *= p_secondaryScale;
			}
			if (width < measured) {
				width = measured;
			}
		}
	}
	return width;
}

// STUB: LEGORACERS2 0x004938f0
LegoU32 __fastcall BeginMenuNetworkDiscovery(GameObject* p_object, void*)
{
	g_controllerRecordingService->m_vtable->m_enumerateHosts(g_controllerRecordingService, NULL);
	*(LegoU32*) ((LegoU8*) p_object + 0x64) = 1;
	*(LegoFloat*) ((LegoU8*) p_object + 0xe4c) = 5.0f;
	RefreshMenuDiscoveredNetworkSessions(p_object, NULL);
	g_controllerRecordingService->m_vtable->m_setUnknownWord0x2c(g_controllerRecordingService, NULL, 0);
	return 1;
}

// STUB: LEGORACERS2 0x00493970
void __fastcall BeginMenuNetworkHostSetup(GameObject* p_object, void*)
{
	*(LegoU32*) ((LegoU8*) p_object + 0x5c) = 3;
	ReleaseSecondaryObject8c(((SecondaryManager*) g_gameController.m_secondaryManager)->m_unk0x8c, NULL);
}

// STUB: LEGORACERS2 0x00493990
void __fastcall RefreshMenuDiscoveredNetworkSessions(GameObject* p_object, void*)
{
	LegoS32* count = (LegoS32*) ((LegoU8*) p_object + 0xb58);
	*count = 5;
	g_controllerRecordingService->m_vtable
		->m_copySessionRecords(g_controllerRecordingService, NULL, (LegoU8*) p_object + 0x89c, count);
}

// STUB: LEGORACERS2 0x004b9280
static LegoBool32 __fastcall AreMenuControllersReady(InputControllerManager* p_manager, void*)
{
	LegoBool32 ready = TRUE;
	for (LegoS32 index = 0; index < p_manager->m_controllerCount; ++index) {
		if (p_manager->m_controllers[index]->m_state == 0) {
			ready = FALSE;
		}
	}
	return ready;
}
