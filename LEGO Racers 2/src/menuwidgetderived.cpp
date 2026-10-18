#include "audioservice.h"
#include "gamecontroller.h"
#include "gamecontrollerconfig.h"
#include "menuchoiceselector.h"
#include "menumanager.h"
#include "menutextgroup.h"
#include "menuvolumeslider.h"

#include <math.h>

struct MenuWidgetAnimationTuning {
	LegoU32 m_unk0x00;
	LegoU32 m_unk0x04;
	LegoU32 m_unk0x08;
	LegoFloat m_markerRadius;
	LegoFloat m_unk0x10;
	LegoFloat m_unk0x14;
	LegoFloat m_unk0x18;
	LegoFloat m_unk0x1c;
};
DECOMP_SIZE_ASSERT(MenuWidgetAnimationTuning, 0x20);

struct MenuWidgetChoiceTuning {
	LegoFloat m_unk0x00;
	LegoFloat m_unk0x04;
	LegoFloat m_unk0x08;
	LegoFloat m_unk0x0c;
	LegoFloat m_unk0x10;
	LegoFloat m_unk0x14;
	LegoFloat m_unk0x18;
	LegoFloat m_unk0x1c;
};
DECOMP_SIZE_ASSERT(MenuWidgetChoiceTuning, 0x20);

// GLOBAL: LEGORACERS2 0x005611e8
MenuWidgetAnimationTuning g_menuWidgetAnimationTuning;
// GLOBAL: LEGORACERS2 0x00561218
MenuWidgetChoiceTuning g_menuWidgetChoiceTuning;
// GLOBAL: LEGORACERS2 0x005611e0
LegoFloat g_menuWidgetLabelPositionTuning[2];
// GLOBAL: LEGORACERS2 0x00561714
LegoU32 g_menuChoiceColorA;
// GLOBAL: LEGORACERS2 0x00561238
LegoU32 g_menuChoiceColorB;

static void __fastcall MenuVolumeUpdateBoundsBridge(MenuWidgetBase* p_widget, void*);
static void __fastcall MenuVolumeSetFontBridge(MenuWidgetBase* p_widget, void*, BitmapFont* p_font);
static void __fastcall MenuVolumeDestroyBridge(MenuWidgetBase* p_widget, void*);
static LegoU32 __fastcall MenuVolumeHandleInputBridge(
	MenuWidgetBase* p_widget,
	void*,
	LegoU32* p_result,
	const LegoU32* p_input
);
static LegoU32 __fastcall MenuWidgetDerivedZeroTwoArgumentsBridge(MenuWidgetBase*, void*, LegoU32*, const LegoU32*);
static void __fastcall MenuVolumeSetClipBridge(
	MenuWidgetBase* p_widget,
	void*,
	const LegoFloat* p_topLeft,
	const LegoFloat* p_bottomRight
);
static void __fastcall MenuChoiceUpdateBoundsBridge(MenuWidgetBase* p_widget, void*);
static void __fastcall MenuChoiceDestroyBridge(MenuWidgetBase* p_widget, void*);
static LegoU32 __fastcall MenuChoiceHandleInputBridge(
	MenuWidgetBase* p_widget,
	void*,
	LegoU32* p_result,
	const LegoU32* p_input
);
static void __fastcall MenuChoiceSetClipBridge(
	MenuWidgetBase* p_widget,
	void*,
	const LegoFloat* p_topLeft,
	const LegoFloat* p_bottomRight
);
static void __fastcall MenuTextGroupUpdateBoundsBridge(MenuWidgetBase* p_widget, void*);
static void __fastcall MenuTextGroupDestroyBridge(MenuWidgetBase* p_widget, void*);
static void __fastcall MenuTextGroupSetClipBridge(
	MenuWidgetBase* p_widget,
	void*,
	const LegoFloat* p_topLeft,
	const LegoFloat* p_bottomRight
);

const MenuWidgetDispatch g_menuVolumeSliderDispatch = {
	&MenuVolumeUpdateBoundsBridge,
	&MenuVolumeSetFontBridge,
	&MenuWidgetDerivedNoOp,
	&MenuVolumeDestroyBridge,
	&MenuVolumeHandleInputBridge,
	&MenuVolumeSetClipBridge
};

const MenuWidgetDispatch g_menuChoiceSelectorDispatch = {
	&MenuChoiceUpdateBoundsBridge,
	&MenuVolumeSetFontBridge,
	&MenuWidgetDerivedNoOp,
	&MenuChoiceDestroyBridge,
	&MenuChoiceHandleInputBridge,
	&MenuChoiceSetClipBridge
};

const MenuWidgetDispatch g_menuTextGroupDispatch = {
	&MenuTextGroupUpdateBoundsBridge,
	&MenuVolumeSetFontBridge,
	&MenuWidgetDerivedNoOp,
	&MenuTextGroupDestroyBridge,
	&MenuWidgetDerivedZeroTwoArgumentsBridge,
	&MenuTextGroupSetClipBridge
};

// STUB: LEGORACERS2 0x00419b80
void InitializeMenuWidgetAnimationTuning()
{
	g_menuWidgetAnimationTuning.m_unk0x00 = 0;
	g_menuWidgetAnimationTuning.m_unk0x04 = 0;
	g_menuWidgetAnimationTuning.m_unk0x08 = 0;
	g_menuWidgetAnimationTuning.m_markerRadius = -0.0651041642f;
	g_menuWidgetAnimationTuning.m_unk0x10 = -0.0859400034f;
	g_menuWidgetAnimationTuning.m_unk0x14 = -0.1149f;
	g_menuWidgetAnimationTuning.m_unk0x18 = 0.0859400034f;
	g_menuWidgetAnimationTuning.m_unk0x1c = -0.1149f;
}

// FUNCTION: LEGORACERS2 0x00419bf0
void InitializeMenuWidgetChoiceTuning()
{
	g_menuWidgetChoiceTuning.m_unk0x00 = 0.25f;
	g_menuWidgetChoiceTuning.m_unk0x04 = 1.0f / 3.0f;
	g_menuWidgetChoiceTuning.m_unk0x08 = 0.03125f;
	g_menuWidgetChoiceTuning.m_unk0x0c = 0.0416666679f;
	g_menuWidgetChoiceTuning.m_unk0x10 = 0.0625f;
	g_menuWidgetChoiceTuning.m_unk0x14 = 0.0833333358f;
	g_menuWidgetChoiceTuning.m_unk0x18 = 0.0625f;
	g_menuWidgetChoiceTuning.m_unk0x1c = 0.0833333358f;
}

// FUNCTION: LEGORACERS2 0x00419c60
void InitializeMenuWidgetLabelPositionTuning()
{
	g_menuWidgetLabelPositionTuning[0] = 0.0f;
	g_menuWidgetLabelPositionTuning[1] = 0.127f;
}

// STUB: LEGORACERS2 0x00419c80
MenuVolumeSlider::MenuVolumeSlider() : MenuWidgetBase(-1)
{
	m_vtable = &g_menuVolumeSliderDispatch;
	m_value = 0.0f;
	m_step = 0.0f;
	m_valueLabel = NULL;
	for (LegoU32 index = 0; index < 4; ++index) {
		m_sprites[index] = NULL;
	}
}

// FUNCTION: LEGORACERS2 0x00419fa0
void MenuVolumeSlider::Destroy()
{
	ReleaseChildSprites();
	if (m_valueLabel != NULL) {
		m_valueLabel->Release();
		m_valueLabel = NULL;
	}
	for (LegoU32 index = 0; index < 4; ++index) {
		if (m_sprites[index] != NULL) {
			m_sprites[index]->Release();
			m_sprites[index] = NULL;
		}
	}
	ResetState();
}

// STUB: LEGORACERS2 0x00419ff0
LegoU32 MenuVolumeSlider::HandleInput(LegoU32* p_result, const LegoU32* p_input)
{
	*p_result = 0xffffffff;
	LegoFloat oldValue = m_value;
	LegoS32 direction = GetNavigationDirection(p_input, 0);
	if (direction != 0) {
		m_value = (LegoFloat) direction * m_step + m_value;
	}
	if (!(m_value >= 0.0f)) {
		m_value = 0.0f;
	}
	else if (m_value > 1.0f) {
		m_value = 1.0f;
	}
	if (m_value < oldValue || m_value > oldValue) {
		UpdateMarker();
		AudioService* audio;
		if ((m_audioTargets & 1) != 0) {
			audio = GetAudioService(TRUE);
			union {
				LegoFloat m_float;
				LegoU32 m_bits;
			} value;
			value.m_float = m_value;
			audio->m_vtable->m_setMixerVolume(audio, NULL, m_value, value.m_bits);
		}
		if ((m_audioTargets & 2) != 0) {
			audio = GetAudioService(TRUE);
			audio->m_vtable->m_setVolumes(audio, NULL, m_value, m_value);
		}
		*p_result = 3;
	}
	return 0;
}

// STUB: LEGORACERS2 0x0041a0c0
void MenuVolumeSlider::UpdateMarker()
{
	LegoFloat angle = (-0.785399973f - 0.785399973f) * m_value + 0.785399973f;
	LegoFloat position[2];
	position[0] = (LegoFloat) sin(angle) * g_menuWidgetAnimationTuning.m_markerRadius * 0.75f + m_originX;
	position[1] = (LegoFloat) cos(angle) * g_menuWidgetAnimationTuning.m_markerRadius + m_originY;
	m_sprites[1]->SetPosition(position);
}

// FUNCTION: LEGORACERS2 0x0041a120
void MenuVolumeSlider::SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight)
{
	SetChildClipRectangle(p_topLeft, p_bottomRight);
	if (m_valueLabel != NULL) {
		m_valueLabel->SetClipRectangle(p_topLeft, p_bottomRight);
	}
	for (LegoU32 index = 0; index < 4; ++index) {
		if (m_sprites[index] != NULL) {
			m_sprites[index]->SetClipRectangle(p_topLeft, p_bottomRight);
		}
	}
}

// STUB: LEGORACERS2 0x0041a170
void MenuVolumeSlider::UpdateBounds()
{
	MenuWidgetBase::UpdateBounds();
	LegoFloat left = m_valueLabel->m_positionX;
	LegoFloat top = m_valueLabel->m_positionY;
	LegoFloat right = left + m_valueLabel->m_measuredWidth;
	LegoFloat bottom = top + m_valueLabel->GetLineHeight();
	if (left < m_bounds[0]) {
		m_bounds[0] = left;
	}
	if (top < m_bounds[1]) {
		m_bounds[1] = top;
	}
	if (m_bounds[2] < right) {
		m_bounds[2] = right;
	}
	if (m_bounds[3] < bottom) {
		m_bounds[3] = bottom;
	}
	for (LegoU32 index = 0; index < 4; ++index) {
		GlobalOverlaySprite* sprite = m_sprites[index];
		LegoFloat size[2];
		LegoFloat spriteLeft = sprite->m_left;
		LegoFloat spriteTop = sprite->m_top;
		sprite->GetSize(size);
		if (spriteLeft < m_bounds[0]) {
			m_bounds[0] = spriteLeft;
		}
		if (spriteTop < m_bounds[1]) {
			m_bounds[1] = spriteTop;
		}
		if (m_bounds[2] < spriteLeft + size[0]) {
			m_bounds[2] = spriteLeft + size[0];
		}
		if (m_bounds[3] < spriteTop + size[1]) {
			m_bounds[3] = spriteTop + size[1];
		}
	}
}

// STUB: LEGORACERS2 0x0041b080
MenuChoiceSelector::MenuChoiceSelector() : MenuWidgetBase(-1)
{
	m_vtable = &g_menuChoiceSelectorDispatch;
	m_positionX = -1.0f;
	m_positionY = -1.0f;
	m_selectedOption = -1;
	for (LegoU32 index = 0; index < 5; ++index) {
		m_optionLabels[index] = NULL;
	}
}

// FUNCTION: LEGORACERS2 0x0041b510
void MenuChoiceSelector::Destroy()
{
	ReleaseChildSprites();
	for (LegoS32 index = 4; index >= 0; --index) {
		if (m_optionLabels[index] != NULL) {
			m_optionLabels[index]->Release();
			m_optionLabels[index] = NULL;
		}
	}
	ResetState();
}

// STUB: LEGORACERS2 0x0041b550
LegoU32 MenuChoiceSelector::HandleInput(LegoU32* p_result, const LegoU32* p_input)
{
	*p_result = 0xffffffff;
	LegoS32 oldSelection = m_selection;
	LegoU32 result = 0;
	LegoS32 direction = GetNavigationDirection(p_input, 1);
	if (direction == -1) {
		m_selection = 1;
	}
	else if (direction == 1) {
		m_selection = 0;
	}

	struct SecondaryManagerMenuState {
		undefined m_prefix[0x7c];
		MenuNavigationState* m_navigation;
	};
	SecondaryManagerMenuState* manager = (SecondaryManagerMenuState*) g_gameController.m_secondaryManager;
	MenuNavigationState* navigation = manager->m_navigation;
	if (navigation != NULL && navigation->m_active != 0) {
		LegoS32 hovered = -1;
		if (navigation->IsRecordUnderPointer(m_optionLabels[0])) {
			hovered = 1;
		}
		else if (navigation->IsRecordUnderPointer(m_optionLabels[2])) {
			hovered = 0;
		}
		m_selectedOption = hovered;
		if (m_selectedOption != -1) {
			navigation->ReadDirectionInput(1);
		}
		LegoFloat pointerPosition[2];
		navigation->GetPointerPosition(pointerPosition);
		if (fabsf(m_positionX - pointerPosition[0]) > 0.0001f || fabsf(m_positionY - pointerPosition[1]) > 0.0001f) {
			if (hovered != -1) {
				m_selection = hovered;
			}
			m_positionX = pointerPosition[0];
			m_positionY = pointerPosition[1];
		}
		if ((navigation->m_verticalFlags & 1) != 0 && hovered != -1) {
			m_selection = hovered;
			result = 2;
		}
	}
	if (m_selection != oldSelection) {
		MenuManager* managerSettings = (MenuManager*) g_gameController.m_menuManager;
		LegoU32 selectionColor = (~managerSettings->m_unk0x218 & 1) == 0 ? g_menuChoiceColorA : g_menuChoiceColorB;
		LegoU32 clearColor = 0xffffffff;
		LegoU32 selectedIndex = m_selection != 0 ? 0 : 2;
		m_optionLabels[selectedIndex]->SetColor(&selectionColor);
		m_optionLabels[2 - selectedIndex]->SetColor(&clearColor);
		m_optionLabels[selectedIndex]->m_animationFlags = 0x40;
		m_optionLabels[2 - selectedIndex]->m_animationFlags = 0;
		*p_result = 3;
	}
	return result;
}

// FUNCTION: LEGORACERS2 0x0041b740
void MenuChoiceSelector::SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight)
{
	SetChildClipRectangle(p_topLeft, p_bottomRight);
	for (LegoU32 index = 0; index < 5; ++index) {
		if (m_optionLabels[index] != NULL) {
			m_optionLabels[index]->SetClipRectangle(p_topLeft, p_bottomRight);
		}
	}
}

// STUB: LEGORACERS2 0x0041b780
void MenuChoiceSelector::UpdateBounds()
{
	MenuWidgetBase::UpdateBounds();
	for (LegoU32 index = 0; index < 5; ++index) {
		GlobalOverlayRecord* record = m_optionLabels[index];
		if (record->m_text[0] != 0) {
			LegoFloat left = record->m_positionX;
			LegoFloat top = record->m_positionY;
			LegoFloat right = left + record->m_measuredWidth;
			LegoFloat bottom = top + record->GetLineHeight();
			if (left < m_bounds[0]) {
				m_bounds[0] = left;
			}
			if (top < m_bounds[1]) {
				m_bounds[1] = top;
			}
			if (m_bounds[2] < right) {
				m_bounds[2] = right;
			}
			if (m_bounds[3] < bottom) {
				m_bounds[3] = bottom;
			}
		}
	}
}

// STUB: LEGORACERS2 0x0041c210
void InitializeMenuChoiceColorA()
{
	g_menuChoiceColorA = 0xffff6a62;
}

// STUB: LEGORACERS2 0x0041c240
void InitializeMenuChoiceColorB()
{
	g_menuChoiceColorB = 0xff626ac8;
}

// STUB: LEGORACERS2 0x00426430
MenuTextGroup::MenuTextGroup() : MenuWidgetBase(-1)
{
	m_vtable = &g_menuTextGroupDispatch;
	for (LegoU32 index = 0; index < 6; ++index) {
		m_records[index] = NULL;
	}
}

// FUNCTION: LEGORACERS2 0x00426660
void MenuTextGroup::Destroy()
{
	for (LegoS32 index = 5; index >= 0; --index) {
		if (m_records[index] != NULL) {
			m_records[index]->Release();
			m_records[index] = NULL;
		}
	}
	ResetState();
}

// FUNCTION: LEGORACERS2 0x00426690
void MenuTextGroup::SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight)
{
	SetChildClipRectangle(p_topLeft, p_bottomRight);
	for (LegoU32 index = 0; index < 6; ++index) {
		if (m_records[index] != NULL) {
			m_records[index]->SetClipRectangle(p_topLeft, p_bottomRight);
		}
	}
}

// STUB: LEGORACERS2 0x004266d0
void MenuTextGroup::UpdateBounds()
{
	m_bounds[0] = 1.0f;
	m_bounds[1] = 1.0f;
	m_bounds[2] = 0.0f;
	m_bounds[3] = 0.0f;
	for (LegoU32 index = 0; index < 6; ++index) {
		GlobalOverlayRecord* record = m_records[index];
		if (record != NULL && record->m_text[0] != 0) {
			LegoFloat topLeft[2];
			LegoFloat bottomRight[2];
			record->GetRectangle(topLeft, bottomRight);
			if (topLeft[0] < m_bounds[0]) {
				m_bounds[0] = topLeft[0];
			}
			if (topLeft[1] < m_bounds[1]) {
				m_bounds[1] = topLeft[1];
			}
			if (m_bounds[2] < bottomRight[0]) {
				m_bounds[2] = bottomRight[0];
			}
			if (m_bounds[3] < bottomRight[1]) {
				m_bounds[3] = bottomRight[1];
			}
		}
	}
}

static void __fastcall MenuVolumeUpdateBoundsBridge(MenuWidgetBase* p_widget, void*)
{
	((MenuVolumeSlider*) p_widget)->UpdateBounds();
}

static void __fastcall MenuVolumeSetFontBridge(MenuWidgetBase* p_widget, void*, BitmapFont* p_font)
{
	SetMenuWidgetFont(p_widget, NULL, p_font);
}

void __fastcall MenuWidgetDerivedNoOp(MenuWidgetBase*, void*)
{
}

static void __fastcall MenuVolumeDestroyBridge(MenuWidgetBase* p_widget, void*)
{
	((MenuVolumeSlider*) p_widget)->Destroy();
}

static LegoU32 __fastcall MenuVolumeHandleInputBridge(
	MenuWidgetBase* p_widget,
	void*,
	LegoU32* p_result,
	const LegoU32* p_input
)
{
	return ((MenuVolumeSlider*) p_widget)->HandleInput(p_result, p_input);
}

static LegoU32 __fastcall MenuWidgetDerivedZeroTwoArgumentsBridge(MenuWidgetBase*, void*, LegoU32*, const LegoU32*)
{
	return 0;
}

static void __fastcall MenuVolumeSetClipBridge(
	MenuWidgetBase* p_widget,
	void*,
	const LegoFloat* p_topLeft,
	const LegoFloat* p_bottomRight
)
{
	((MenuVolumeSlider*) p_widget)->SetClipRectangle(p_topLeft, p_bottomRight);
}

static void __fastcall MenuChoiceUpdateBoundsBridge(MenuWidgetBase* p_widget, void*)
{
	((MenuChoiceSelector*) p_widget)->UpdateBounds();
}

static void __fastcall MenuChoiceDestroyBridge(MenuWidgetBase* p_widget, void*)
{
	((MenuChoiceSelector*) p_widget)->Destroy();
}

static LegoU32 __fastcall MenuChoiceHandleInputBridge(
	MenuWidgetBase* p_widget,
	void*,
	LegoU32* p_result,
	const LegoU32* p_input
)
{
	return ((MenuChoiceSelector*) p_widget)->HandleInput(p_result, p_input);
}

static void __fastcall MenuChoiceSetClipBridge(
	MenuWidgetBase* p_widget,
	void*,
	const LegoFloat* p_topLeft,
	const LegoFloat* p_bottomRight
)
{
	((MenuChoiceSelector*) p_widget)->SetClipRectangle(p_topLeft, p_bottomRight);
}

static void __fastcall MenuTextGroupUpdateBoundsBridge(MenuWidgetBase* p_widget, void*)
{
	((MenuTextGroup*) p_widget)->UpdateBounds();
}

static void __fastcall MenuTextGroupDestroyBridge(MenuWidgetBase* p_widget, void*)
{
	((MenuTextGroup*) p_widget)->Destroy();
}

static void __fastcall MenuTextGroupSetClipBridge(
	MenuWidgetBase* p_widget,
	void*,
	const LegoFloat* p_topLeft,
	const LegoFloat* p_bottomRight
)
{
	((MenuTextGroup*) p_widget)->SetClipRectangle(p_topLeft, p_bottomRight);
}

struct InitializeMenuWidgetGlobalsAtStartup {
	InitializeMenuWidgetGlobalsAtStartup()
	{
		InitializeMenuWidgetAnimationTuning();
		InitializeMenuWidgetChoiceTuning();
		InitializeMenuWidgetLabelPositionTuning();
		InitializeMenuChoiceColorA();
		InitializeMenuChoiceColorB();
		InitializeMenuPointerOffsets();
	}
};
static InitializeMenuWidgetGlobalsAtStartup g_initializeMenuWidgetGlobalsAtStartup;
