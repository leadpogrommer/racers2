#include "menumanagercurrentaction.h"

#include "application.h"
#include "applicationdispatch.h"
#include "audioservice.h"
#include "clipplanes.h"
#include "commandline.h"
#include "displayconfig.h"
#include "gamecontroller.h"
#include "gamecontrollerconfig.h"
#include "gamecontrollerinitialization.h"
#include "gamecontrollersession.h"
#include "gameobjectrenderprep.h"
#include "gamewindow.h"
#include "localizedstringtable.h"
#include "menucarpreviewselector.h"
#include "menucharacterpreviewselector.h"
#include "menuwidget.h"
#include "modelrendering.h"
#include "renderer.h"
#include "rendererconstructiondispatch.h"
#include "rendererdispatch.h"
#include "rendererlookupstate.h"
#include "secondarymanager.h"

#include <math.h>

extern LegoU32 g_menuChoiceColorA;
extern LegoU32 g_menuChoiceColorB;

struct MenuLanguageChoice {
	LegoU32 m_language;
	LegoU32 m_stringId;
};

static const MenuLanguageChoice g_menuStandardLanguages[] =
	{{0, 576}, {1, 578}, {2, 574}, {4, 579}, {5, 581}, {6, 580}, {7, 582}, {8, 577}};
static const MenuLanguageChoice g_menuDevelopmentLanguages[] = {{3, 574}};

struct MenuDisplayPolicyView {
	undefined m_prefix[0xdec];
	LegoU32 m_value;
};

static LegoS32 RoundMenuFloatToInt(LegoFloat p_value)
{
	volatile LegoFloat storedValue = p_value;
	LegoS32 result;
	__asm {
		fld storedValue
		fistp result
	}
	return result;
}

void __fastcall UpdateMenuChoiceSelectorHover(MenuChoiceSelector*, void*, LegoS32*, MenuNavigationState*);

namespace
{
struct MenuSelectorNavigationView {
	undefined m_prefix[0x7c];
	MenuNavigationState* m_navigation;
};

LegoFloat __fastcall ReadMenuWidgetFloat(
	MenuManager* p_manager,
	LegoU32 p_index,
	LegoU32 p_id,
	LegoU32 p_offset,
	LegoFloat p_default
)
{
	MenuWidgetBase* widget = p_manager->m_widgets[p_index];
	if (widget->m_initialValue == (LegoS32) p_id) {
		return *(LegoFloat*) ((LegoU8*) widget + p_offset);
	}
	return p_default;
}

LegoU32 __fastcall ReadMenuWidgetWord(
	MenuManager* p_manager,
	LegoU32 p_index,
	LegoU32 p_id,
	LegoU32 p_offset,
	LegoU32 p_default
)
{
	MenuWidgetBase* widget = p_manager->m_widgets[p_index];
	if (widget->m_initialValue == (LegoS32) p_id) {
		return *(LegoU32*) ((LegoU8*) widget + p_offset);
	}
	return p_default;
}

// Values in this helper group are read by id from the corresponding widget.
LegoFloat __fastcall MenuManagerGetFloatAtZero(MenuManager* p_manager, void*, LegoU32 p_id)
{
	return ReadMenuWidgetFloat(p_manager, 0, p_id, 0x48, 0.0f);
}

LegoFloat __fastcall MenuManagerGetFloatAtThreeToSix(MenuManager* p_manager, void*, LegoU32 p_id)
{
	for (LegoU32 i = 3; i < 7; ++i) {
		MenuWidgetBase* widget = p_manager->m_widgets[i];
		if (widget->m_initialValue == (LegoS32) p_id) {
			return *(LegoFloat*) ((LegoU8*) widget + 0x54);
		}
	}
	return 0.0f;
}

LegoU32 __fastcall MenuManagerGetWordAtOne(MenuManager* p_manager, void*, LegoU32 p_id)
{
	return ReadMenuWidgetWord(p_manager, 1, p_id, 0x48, 0);
}

LegoU32 __fastcall MenuManagerGetWordAtSeven(MenuManager* p_manager, void*, LegoU32 p_id)
{
	return ReadMenuWidgetWord(p_manager, 7, p_id, 0x48, 0);
}

LegoU32 __fastcall MenuManagerGetWordAtEight(MenuManager* p_manager, void*, LegoU32 p_id)
{
	return ReadMenuWidgetWord(p_manager, 8, p_id, 0x50, 0);
}

LegoU32 __fastcall MenuManagerGetWordAtNine(MenuManager* p_manager, void*, LegoU32 p_id)
{
	return ReadMenuWidgetWord(p_manager, 9, p_id, 0x9c, 0);
}

LegoS32 __fastcall MenuManagerGetCharacterChoice(
	MenuManager* p_manager,
	void*,
	LegoU32 p_id,
	MenuCharacterPreviewChoice* p_output
)
{
	MenuCharacterPreviewSelector* widget = (MenuCharacterPreviewSelector*) p_manager->m_widgets[10];
	if (widget->m_initialValue != (LegoS32) p_id) {
		return 0xffffffff;
	}
	return widget->GetChoice(p_output);
}

LegoS32 __fastcall MenuManagerGetCarChoice(MenuManager* p_manager, void*, LegoU32 p_id, LegoU32* p_output)
{
	MenuCarPreviewSelector* widget = (MenuCarPreviewSelector*) p_manager->m_widgets[11];
	if (widget->m_initialValue != (LegoS32) p_id) {
		return 0;
	}
	LegoS32 selectedValue = 0;
	LegoS32 result = widget->GetChoice(&selectedValue);
	if (selectedValue != 0) {
		for (LegoU32 i = 0; i < 0xbd; ++i) {
			p_output[i] = ((LegoU32*) selectedValue)[i];
		}
	}
	return result;
}

} // namespace

// STUB: LEGORACERS2 0x0041b0d0
void __fastcall InitializeMenuChoiceSelectorFromMode(
	MenuChoiceSelector* p_selector,
	void*,
	LegoS32 p_initialValue,
	const LegoFloat* p_position,
	LegoS32 p_selection,
	LegoS32 p_firstString,
	LegoS32 p_secondString,
	LegoS32 p_leftString,
	LegoS32 p_rightString,
	LegoFloat p_maxScale,
	LegoU32 p_createArrows,
	LegoU32 p_state
)
{
	p_selector->BeginState();
	p_selector->m_initialValue = p_initialValue;
	p_selector->m_unk0x14 = p_state;
	p_selector->m_selection = p_selection;

	LegoS32 listId = p_createArrows != 0 ? 11 : 9;
	LegoU32 parameters[13];
	parameters[0] = (LegoU32) p_selector->m_font;
	parameters[1] = 0xffffffff;
	// Retail leaves the initial position words (2/3) untouched.
	parameters[4] = parameters[5] = 0;
	parameters[6] = parameters[7] = 0x3f800000;
	parameters[8] = 0;
	parameters[9] = listId;
	parameters[10] = 0x3f800000;
	parameters[11] = parameters[12] = 0;
	for (LegoU32 allocateIndex = 0; allocateIndex < 5; ++allocateIndex) {
		p_selector->m_optionLabels[allocateIndex] = g_globalOverlayManager.AllocateRecord(parameters, -1);
	}

	GlobalOverlayRecord* leftArrow = p_selector->m_optionLabels[3];
	GlobalOverlayRecord* rightArrow = p_selector->m_optionLabels[4];
	GlobalOverlayRecord* mainLabel = p_selector->m_optionLabels[2];
	GlobalOverlayRecord* leftLabel = p_selector->m_optionLabels[0];
	GlobalOverlayRecord* rightLabel = p_selector->m_optionLabels[1];
	leftArrow->SetScale(1.0f);
	leftArrow->InitializeTextByIndex(p_leftString, 8, 0.0f);
	if (p_maxScale < leftArrow->m_measuredWidth) {
		leftArrow->SetScale(p_maxScale / leftArrow->m_measuredWidth);
		leftArrow->InitializeTextByIndex(p_leftString, 8, 0.0f);
	}
	rightArrow->SetScale(1.0f);
	rightArrow->InitializeTextByIndex(p_rightString, 8, 0.0f);
	if (p_maxScale < rightArrow->m_measuredWidth) {
		rightArrow->SetScale(p_maxScale / rightArrow->m_measuredWidth);
		rightArrow->InitializeTextByIndex(p_rightString, 8, 0.0f);
	}

	leftLabel->InitializeTextByIndex(p_secondString == -1 ? 2 : p_secondString, 0, 0.0f);
	rightLabel->InitializeNarrowText("/", 0, 0.0f);
	mainLabel->InitializeTextByIndex(p_firstString == -1 ? 3 : p_firstString, 0, 0.0f);
	MenuManager* menu = (MenuManager*) g_gameController.m_menuManager;
	LegoU32 selectedColor = ((~menu->m_unk0x218 & 1) == 0) ? g_menuChoiceColorA : g_menuChoiceColorB;
	LegoU32 clearColor = 0xffffffff;
	if (p_selection != 0) {
		leftLabel->SetColor(&selectedColor);
		mainLabel->SetColor(&clearColor);
		leftLabel->m_animationFlags = 0x40;
		mainLabel->m_animationFlags = 0;
	}
	else {
		leftLabel->SetColor(&clearColor);
		mainLabel->SetColor(&selectedColor);
		mainLabel->m_animationFlags = 0x40;
		leftLabel->m_animationFlags = 0;
	}

	LegoFloat widths[3] = {leftLabel->m_measuredWidth, rightLabel->m_measuredWidth, mainLabel->m_measuredWidth};
	LegoFloat totalWidth = 0.0f;
	for (LegoU32 widthIndex = 0; widthIndex < 3; ++widthIndex) {
		totalWidth += widths[widthIndex];
	}
	LegoFloat x = p_position[0];
	LegoFloat y = p_position[1];
	if (p_leftString != -1) {
		LegoFloat position[2] = {x, y};
		leftArrow->SetPosition(position, 9);
		y += 0.05f;
	}
	if (p_rightString != -1) {
		LegoFloat position[2] = {x, y};
		rightArrow->SetPosition(position, 9);
		y += 0.05f;
	}
	x -= totalWidth * 0.5f;
	for (LegoU32 positionIndex = 0; positionIndex < 3; ++positionIndex) {
		LegoFloat position[2] = {x, y};
		p_selector->m_optionLabels[positionIndex]->SetPosition(position, 0xffffffff);
		x += widths[positionIndex];
	}

	LegoFloat topLeft[2];
	LegoFloat bottomRight[2];
	leftLabel->GetRectangle(topLeft, bottomRight);
	LegoFloat left = topLeft[0];
	mainLabel->GetRectangle(topLeft, bottomRight);
	LegoFloat lineHeight = leftLabel->GetLineHeight();
	LegoFloat center[2] = {(bottomRight[0] + left) * 0.5f, topLeft[1] + lineHeight * 0.5f};
	p_selector->CreateChildSprites(center, center[0] - left, listId, 1.0f);

	MenuSelectorNavigationView* owner = (MenuSelectorNavigationView*) g_gameController.m_secondaryManager;
	if (owner->m_navigation != NULL && owner->m_navigation->m_active != 0) {
		owner->m_navigation->GetPointerPosition(&p_selector->m_positionX);
		UpdateMenuChoiceSelectorHover(p_selector, NULL, &p_selector->m_selectedOption, owner->m_navigation);
	}
	p_selector->m_vtable->m_updateBounds(p_selector, NULL);
}

// FUNCTION: LEGORACERS2 0x0041b6f0
void __fastcall UpdateMenuChoiceSelectorHover(
	MenuChoiceSelector* p_selector,
	void*,
	LegoS32* p_output,
	MenuNavigationState* p_navigation
)
{
	*p_output = -1;
	if (p_navigation->IsRecordUnderPointer(p_selector->m_optionLabels[0])) {
		*p_output = 1;
	}
	else if (p_navigation->IsRecordUnderPointer(p_selector->m_optionLabels[2])) {
		*p_output = 0;
	}
}

// STUB: LEGORACERS2 0x00422710
void __fastcall UpdateMenuManagerCurrentAction(MenuManager* p_manager, void*)
{
	GameControllerSettings* settings = g_gameControllerSettings;
	GameControllerSession* session = g_gameControllerSession;
	LegoU32 action = *(LegoU32*) ((LegoU8*) p_manager + 0x44 + p_manager->m_unk0x198 * 0x14);
	switch (action) {
	case 10: {
		LegoS32 width = RoundMenuFloatToInt(GetMenuManagerFloatOption(p_manager, NULL, 0) * 9.0f) + 1;
		LegoS32 height = RoundMenuFloatToInt(GetMenuManagerFloatOption(p_manager, NULL, 1) * 9.0f) + 1;
		LegoS32 depth = RoundMenuFloatToInt(GetMenuManagerFloatOption(p_manager, NULL, 2) * 4.0f) + 1;
		LegoBool32 changed = FALSE;
		if (width != (LegoS32) g_gameController.m_graphicsQuality || height != (LegoS32) g_gameController.m_unk0x340 ||
			depth != (LegoS32) g_gameController.m_unk0x344) {
			g_gameController.m_graphicsQuality = width;
			g_gameController.m_unk0x340 = height;
			g_gameController.m_unk0x344 = depth;
			ApplyMenuGraphicsQuality(g_displayRenderer, NULL, width, height, depth);
			changed = TRUE;
		}
		LegoS32 refreshCount = *(LegoS32*) ((LegoU8*) g_gameController.m_secondaryManager + 0x4bc);
		if (refreshCount > 1) {
			LegoS32 refreshIndex =
				RoundMenuFloatToInt(GetMenuManagerFloatOption(p_manager, NULL, 3) * (refreshCount - 1)) * 3 + 0x99;
			LegoU32* refresh = (LegoU32*) ((LegoU8*) g_gameController.m_secondaryManager + refreshIndex * 4);
			if ((refresh[0] != g_displayConfiguration->m_width || refresh[1] != g_displayConfiguration->m_height ||
				 refresh[2] != g_displayConfiguration->m_bitDepth) &&
				UpdateMenuResolution((SecondaryManager*) g_gameController.m_secondaryManager, NULL, refresh) != 0) {
				goto reset_resolution;
			}
		}
		if (changed != FALSE) {
		reset_resolution:
			g_gameController.m_unk0x348 = 0;
			g_gameController.m_unk0x34c = 0;
			return;
		}
		break;
	}
	case 0x0b: {
		Application* app = GetApplication();
		LegoU32 development = app->m_vtable->m_value0x7c(app, NULL);
		const MenuLanguageChoice* modeList = development != 0 ? g_menuDevelopmentLanguages : g_menuStandardLanguages;
		LegoU32 mode = modeList[GetMenuManagerWordAtNine(p_manager, NULL, 0)].m_language;
		if ((LegoS32) mode != settings->m_language) {
			settings->m_language = mode;
			ApplyMenuLanguage((SecondaryManager*) g_gameController.m_secondaryManager, NULL, mode);
			return;
		}
		break;
	}
	case 0x0c:
		settings->m_unk0xa4 = GetMenuManagerWordOption(p_manager, NULL, 0);
		return;
	case 0x0d:
	case 0x27:
		settings->m_effectGain = GetMenuManagerFloatValue(p_manager, NULL, 0);
		break;
	case 0x0e:
		settings->m_musicGain = GetMenuManagerFloatValue(p_manager, NULL, 0);
		return;
	case 0x0f: {
		LegoU32 mode = GetMenuManagerWordAtNine(p_manager, NULL, 0);
		if (mode != settings->m_audioQuality) {
			settings->m_audioQuality = mode;
			ApplySavedAudioQuality(NULL, NULL);
			StartControllerMusic(&g_gameController, NULL, 0, 1);
			return;
		}
		break;
	}
	case 0x12: {
		LegoU32 side = ~p_manager->m_unk0x218 & 1;
		LegoU32 value = GetMenuManagerWordOption(p_manager, NULL, 0);
		*(LegoU32*) ((LegoU8*) settings + 0xa8 + side * 4) = value;
		return;
	}
	case 0x1d: {
		LegoU32 value = GetMenuManagerWordAtEight(p_manager, NULL, 0);
		LegoS32 gameMode = *(LegoS32*) ((LegoU8*) g_gameControllerSession + 8);
		if (gameMode == 2) {
			*(LegoU32*) ((LegoU8*) g_gameControllerSession +
						 *(LegoS32*) ((LegoU8*) g_gameControllerSession + 0x1a70) * 4 + 0x1ac8) = value;
			return;
		}
		*(LegoU32*) ((LegoU8*) settings + ((*(LegoS32*) ((LegoU8*) g_gameControllerSession + 0xc) + 2) * 5 +
										   *(LegoS32*) ((LegoU8*) g_gameControllerSession + 0x10)) *
											  4) = value;
		return;
	}
	case 0x20: {
		LegoU32 side = ~p_manager->m_unk0x218 & 1;
		LegoU8* selection = (LegoU8*) g_gameControllerSession + side * 0x344 + 0x370;
		LegoU32 value = GetMenuManagerCharacterChoice(p_manager, NULL, 0, (MenuCharacterPreviewChoice*) selection);
		*(LegoU32*) ((LegoU8*) settings + 0x7f8c + side * 4) = value;
		if (*(LegoU32*) (selection + 4) < 5) {
			*(LegoU32*) (selection + 0x10) = 1;
		}
		else {
			*(LegoU32*) (selection + 0x10) = 0;
		}
		return;
	}
	case 0x21: {
		LegoU32 side = ~p_manager->m_unk0x218 & 1;
		LegoU32 value = GetMenuManagerCarChoice(
			p_manager,
			NULL,
			0,
			(LegoU32*) ((LegoU8*) g_gameControllerSession + side * 0x344 + 0x6c)
		);
		if (session->m_mode != 4 || session->m_selectedMode != 0) {
			*(LegoU32*) ((LegoU8*) settings + 0x7f94 + side * 4) = value;
			return;
		}
		break;
	}
	case 0x26:
		settings->m_musicGain = GetMenuManagerFloatValue(p_manager, NULL, 0);
		return;
	}
}

// STUB: LEGORACERS2 0x00423770
LegoFloat __fastcall GetMenuManagerFloatValue(MenuManager* p_manager, void*, LegoU32 p_id)
{
	return MenuManagerGetFloatAtZero(p_manager, NULL, p_id);
}

// STUB: LEGORACERS2 0x004237a0
LegoFloat __fastcall GetMenuManagerFloatOption(MenuManager* p_manager, void*, LegoU32 p_id)
{
	return MenuManagerGetFloatAtThreeToSix(p_manager, NULL, p_id);
}

// STUB: LEGORACERS2 0x004237e0
LegoU32 __fastcall GetMenuManagerWordOption(MenuManager* p_manager, void*, LegoU32 p_id)
{
	return MenuManagerGetWordAtSeven(p_manager, NULL, p_id);
}

// STUB: LEGORACERS2 0x00423810
LegoU32 __fastcall GetMenuManagerInitialValue(MenuManager* p_manager, void*, LegoU32 p_id)
{
	return MenuManagerGetWordAtOne(p_manager, NULL, p_id);
}

// STUB: LEGORACERS2 0x00423840
LegoU32 __fastcall GetMenuManagerWordAtEight(MenuManager* p_manager, void*, LegoU32 p_id)
{
	return MenuManagerGetWordAtEight(p_manager, NULL, p_id);
}

// STUB: LEGORACERS2 0x00423870
LegoU32 __fastcall GetMenuManagerWordAtNine(MenuManager* p_manager, void*, LegoU32 p_id)
{
	return MenuManagerGetWordAtNine(p_manager, NULL, p_id);
}

// STUB: LEGORACERS2 0x004238a0
LegoS32 __fastcall GetMenuManagerCharacterChoice(
	MenuManager* p_manager,
	void*,
	LegoU32 p_id,
	MenuCharacterPreviewChoice* p_output
)
{
	return MenuManagerGetCharacterChoice(p_manager, NULL, p_id, p_output);
}

// STUB: LEGORACERS2 0x004238e0
LegoS32 __fastcall GetMenuManagerCarChoice(MenuManager* p_manager, void*, LegoU32 p_id, LegoU32* p_output)
{
	return MenuManagerGetCarChoice(p_manager, NULL, p_id, p_output);
}

// STUB: LEGORACERS2 0x00429620
LegoU32 __fastcall UpdateMenuResolution(SecondaryManager* p_manager, void*, const LegoU32* p_mode)
{
	if (g_displayConfiguration->m_fullscreen == 0) {
		return 0;
	}
	DisplayRenderer* renderer = g_displayRenderer;
	const RendererMainDispatch* dispatch = (const RendererMainDispatch*) renderer->m_vtable;
	LegoU32 drawPolicy = 0xffffffff;
	if (p_manager->m_unk0x64 != 0) {
		drawPolicy = ((MenuDisplayPolicyView*) g_gameController.m_unk0x90)->m_value;
	}
	dispatch->m_setUnknownDrawPolicy(renderer, NULL, drawPolicy);
	Application* app = GetApplication();
	ApplicationDisplaySettings* current = app->m_vtable->m_displaySettings(app, NULL);
	LegoU32 oldWidth = (LegoU32) current->m_width;
	LegoU32 oldHeight = (LegoU32) current->m_height;
	LegoU32 oldDepth = (LegoU32) current->m_bitDepth;
	LegoU32 width = p_mode[0];
	LegoU32 height = p_mode[1];
	LegoU32 depth = p_mode[2];
	LegoBool32 requestedModeSucceeded = dispatch->m_reconfigureDisplayMode(renderer, NULL, width, height, depth, 0);
	if (requestedModeSucceeded == FALSE) {
		LegoBool32 restored = dispatch->m_reconfigureDisplayMode(renderer, NULL, oldWidth, oldHeight, oldDepth, 0);
		if (restored == FALSE) {
			g_deferredWindowDestruction = 1;
			return 0;
		}
	}
	SetGameObjectViewport(
		p_manager->m_unk0x110,
		NULL,
		0,
		0,
		(LegoS32) g_displayConfiguration->m_width,
		(LegoS32) g_displayConfiguration->m_height
	);
	if (requestedModeSucceeded != FALSE) {
		ApplicationDisplaySettings settings;
		settings.m_width = width;
		settings.m_height = height;
		settings.m_bitDepth = depth;
		app = GetApplication();
		app->m_vtable->m_setDisplaySettings(app, NULL, &settings, 0);
	}
	return requestedModeSucceeded;
}

// STUB: LEGORACERS2 0x0042a6b0
void __fastcall ApplyMenuLanguage(SecondaryManager* p_manager, void*, LegoU32 p_mode)
{
	LegoU32 languageValue = p_manager->m_unk0x64 == 0 ? 0 : p_manager->m_unk0x4d4[p_mode];
	if (p_manager->m_unk0x64 == 0 || languageValue != 0) {
		GetLocalizedStringTable()->SelectLanguage((LegoS32) p_mode, (const void*) languageValue, 0x10);
		InitializeLocalizedPlayerNames();
	}
}

// STUB: LEGORACERS2 0x004aa300
void SetModelClipPlanesForDetail(LegoS32 p_detail)
{
	// Retail uses signed comparisons and a wrapped 32-bit integer before FILD.
	LegoU32 value = p_detail < 8 ? (LegoU32) p_detail * 14 + 0x26 : (LegoU32) p_detail * 300 - 0x8ca;
	LegoFloat nearPlane = (LegoFloat) (LegoS32) value;
	g_modelClipNear = nearPlane;
	g_modelClipFar = nearPlane + 50.0f;
}

// STUB: LEGORACERS2 0x004bec30
void __fastcall ApplyMenuGraphicsQuality(
	DisplayRenderer* renderer,
	void*,
	LegoS32 p_detail,
	LegoS32 p_geometryQuality,
	LegoS32 p_textureQuality
)
{
	renderer->m_unk0x4d0 = p_detail;
	renderer->m_unk0x4d4 = p_geometryQuality;
	renderer->m_unk0x4d8 = p_textureQuality;
	SetModelClipPlanesForDetail(p_detail);
	if (renderer->m_unk0x4d0 < 3) {
		g_modelLodDepthThreshold = 2147483648.0f;
		g_unk0x0058a170.m_mode = 1;
	}
	else {
		g_modelLodDepthThreshold = renderer->m_unk0x4d0 <= 8 ? 0.2f : 0.0f;
		g_unk0x0058a170.m_mode = 0;
	}
	const RendererMainDispatch* dispatch = (const RendererMainDispatch*) renderer->m_vtable;
	LegoS32 quality = dispatch->m_getTextureQuality(renderer, NULL);
	dispatch->m_setTextureQuality(renderer, NULL, (LegoU32) quality);
}
