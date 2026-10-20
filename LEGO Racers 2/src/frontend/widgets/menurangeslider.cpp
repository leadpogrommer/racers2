#include "frontend/widgets/menurangeslider.h"

#include "frontend/secondary/secondarymanager.h"
#include "game/controller/gamecontroller.h"

#include <math.h>

static void __fastcall RangeUpdateBounds(MenuWidgetBase* p_widget, void*)
{
	((MenuRangeSlider*) p_widget)->UpdateBounds();
}
static void __fastcall RangeReset(MenuWidgetBase* p_widget, void*)
{
	((MenuRangeSlider*) p_widget)->Reset();
}
static LegoU32 __fastcall RangeInput(MenuWidgetBase* p_widget, void*, LegoU32* p_result, const LegoU32* p_input)
{
	return ((MenuRangeSlider*) p_widget)->HandleInput(p_result, p_input);
}
static void __fastcall RangeClip(MenuWidgetBase* p_widget, void*, const LegoFloat* p_first, const LegoFloat* p_second)
{
	((MenuRangeSlider*) p_widget)->SetClipRectangle(p_first, p_second);
}

const MenuWidgetDispatch g_menuRangeSliderDispatch =
	{&RangeUpdateBounds, &SetMenuWidgetFont, &MenuWidgetDerivedNoOp, &RangeReset, &RangeInput, &RangeClip};

// GLOBAL: LEGORACERS2 0x00561208
LegoFloat g_menuRangeMarkerSize[2];

// FUNCTION: LEGORACERS2 0x0041a2d0
void InitializeMenuRangeMarkerSize()
{
	g_menuRangeMarkerSize[0] = 0.03125f;
	g_menuRangeMarkerSize[1] = 0.041666668f;
}

// STUB: LEGORACERS2 0x0041a2f0
MenuRangeSlider::MenuRangeSlider() : MenuWidgetBase(-1)
{
	m_centerX = m_centerY = m_width = m_value = m_step = m_displayedValue = 0;
	m_dragging = 0;
	m_valueNames = NULL;
	LegoU32 i;
	for (i = 0; i < 3; ++i) {
		m_labels[i] = NULL;
	}
	for (i = 0; i < 4; ++i) {
		m_sprites[i] = NULL;
	}
	m_vtable = &g_menuRangeSliderDispatch;
}

// FUNCTION: LEGORACERS2 0x0041a760
void MenuRangeSlider::Reset()
{
	ReleaseChildSprites();
	LegoS32 i;
	for (i = 2; i >= 0; --i) {
		if (m_labels[i] != NULL) {
			m_labels[i]->Release();
			m_labels[i] = NULL;
		}
	}
	for (i = 3; i >= 0; --i) {
		if (m_sprites[i] != NULL) {
			m_sprites[i]->Release();
			m_sprites[i] = NULL;
		}
	}
	ResetState();
}

// STUB: LEGORACERS2 0x0041a7c0
void MenuRangeSlider::UpdateMarker()
{
	LegoFloat position[2] = {(m_displayedValue - 0.5f) * (m_width - g_menuRangeMarkerSize[0]) + m_centerX, m_centerY};
	m_sprites[3]->SetPosition(position);
}

// STUB: LEGORACERS2 0x0041a810
LegoU32 MenuRangeSlider::HandleInput(LegoU32* p_result, const LegoU32* p_input)
{
	*p_result = 0xffffffff;
	LegoFloat value = m_value;
	MenuNavigationState* navigation =
		(MenuNavigationState*) ((SecondaryManager*) g_gameController.m_secondaryManager)->m_unk0x7c;
	if (navigation != NULL && navigation->m_active != 0) {
		LegoBool32 hit[4];
		LegoBool32 any = FALSE;
		for (LegoU32 i = 0; i < 4; ++i) {
			hit[i] = navigation->IsSpriteUnderPointer(m_sprites[i]) != 0;
			if (hit[i]) {
				any = TRUE;
			}
		}
		if (any) {
			navigation->ReadDirectionInput(1);
		}
		if (m_dragging == 0) {
			if (any && (navigation->m_verticalFlags & 1) != 0) {
				LegoFloat pointerValue = GetPointerValue();
				if (!hit[3]) {
					SnapValue(pointerValue, &value);
				}
				else {
					m_dragging = 1;
					value = pointerValue;
				}
			}
		}
		else {
			navigation->ReadDirectionInput(1);
			LegoFloat pointerValue = GetPointerValue();
			value = pointerValue;
			if ((navigation->m_horizontalFlags & 1) == 0) {
				SnapValue(pointerValue, &value);
				m_dragging = 0;
			}
		}
	}
	if (m_dragging == 0) {
		LegoS32 direction = GetNavigationDirection(p_input, 1);
		if (direction != 0) {
			value = (LegoFloat) direction * m_step + value;
		}
	}
	if (0 <= value) {
		if (1 <= value) {
			value = 1;
		}
	}
	else {
		value = 0;
	}
	if (value != m_value) {
		m_value = value;
		if (m_dragging == 0) {
			*p_result = 3;
		}
		UpdateValueLabel();
	}
	if (m_displayedValue != m_value) {
		if (m_dragging != 0 || fabs(m_value - m_displayedValue) <= m_displayStep) {
			m_displayedValue = m_value;
		}
		else if (m_value - m_displayedValue < 0) {
			m_displayedValue -= m_displayStep;
		}
		else {
			m_displayedValue = m_displayStep + m_displayedValue;
		}
		UpdateMarker();
	}
	return 0;
}

// FUNCTION: LEGORACERS2 0x0041aa50
void MenuRangeSlider::SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight)
{
	SetChildClipRectangle(p_topLeft, p_bottomRight);
	LegoU32 i;
	for (i = 0; i < 3; ++i) {
		if (m_labels[i] != NULL) {
			m_labels[i]->SetClipRectangle(p_topLeft, p_bottomRight);
		}
	}
	for (i = 0; i < 4; ++i) {
		if (m_sprites[i] != NULL) {
			m_sprites[i]->SetClipRectangle(p_topLeft, p_bottomRight);
		}
	}
}

// STUB: LEGORACERS2 0x0041aab0
void MenuRangeSlider::UpdateBounds()
{
	MenuWidgetBase::UpdateBounds();
	for (LegoU32 i = 0; i < 7; ++i) {
		LegoFloat topLeft[2], bottomRight[2];
		if (i < 3) {
			if (m_labels[i]->m_text[0] == 0) {
				continue;
			}
			m_labels[i]->GetRectangle(topLeft, bottomRight);
		}
		else {
			m_sprites[i - 3]->GetRectangle(topLeft, bottomRight);
		}
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

// STUB: LEGORACERS2 0x0041ab60
LegoFloat MenuRangeSlider::GetPointerValue() const
{
	LegoFloat pointer[2];
	MenuNavigationState* navigation =
		(MenuNavigationState*) ((SecondaryManager*) g_gameController.m_secondaryManager)->m_unk0x7c;
	navigation->GetPointerPosition(pointer);
	LegoFloat width = m_width - g_menuRangeMarkerSize[0];
	LegoFloat left = m_centerX - width * 0.5f;
	LegoFloat right = left + width;
	if (pointer[0] <= left) {
		return 0;
	}
	if (right <= pointer[0]) {
		return 1;
	}
	return (pointer[0] - left) / (right - left);
}

// STUB: LEGORACERS2 0x0041abf0
LegoS32 MenuRangeSlider::SnapValue(LegoFloat p_value, LegoFloat* p_result) const
{
	LegoFloat candidate = 0;
	LegoFloat closestDistance = 1000;
	LegoS32 index = 0, closest = 0;
	*p_result = 0;
	do {
		LegoFloat distance = (LegoFloat) fabs(candidate - p_value);
		if (distance < closestDistance) {
			*p_result = candidate;
			closest = index;
			closestDistance = distance;
		}
		candidate += m_step;
		++index;
	} while (candidate <= 1.001f);
	return closest;
}

// STUB: LEGORACERS2 0x0041ac50
void MenuRangeSlider::UpdateValueLabel()
{
	if (m_valueNames != NULL) {
		LegoFloat snapped;
		LegoS32 index = SnapValue(m_value, &snapped);
		m_labels[2]->InitializeNarrowText(m_valueNames[index].m_data, 1, 0);
		m_labels[2]->SetScale(0.35f);
		LegoFloat position[2] = {m_centerX, g_menuRangeMarkerSize[1] * 0.5f + 0.0f + m_centerY};
		m_labels[2]->SetPosition(position, 0xffffffff);
	}
}

struct InitializeMenuRangeGlobalsAtStartup {
	InitializeMenuRangeGlobalsAtStartup() { InitializeMenuRangeMarkerSize(); }
};
static InitializeMenuRangeGlobalsAtStartup g_initializeMenuRangeGlobalsAtStartup;
