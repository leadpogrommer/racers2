#include "menuintegerselector.h"

static void __fastcall IntegerUpdateBounds(MenuWidgetBase* p_widget, void*)
{
	((MenuIntegerSelector*) p_widget)->UpdateBounds();
}
static void __fastcall IntegerReset(MenuWidgetBase* p_widget, void*)
{
	((MenuIntegerSelector*) p_widget)->Reset();
}
static LegoU32 __fastcall IntegerInput(MenuWidgetBase* p_widget, void*, LegoU32* p_result, const LegoU32* p_input)
{
	return ((MenuIntegerSelector*) p_widget)->HandleInput(p_result, p_input);
}
static void __fastcall IntegerClip(MenuWidgetBase* p_widget, void*, const LegoFloat* p_first, const LegoFloat* p_second)
{
	((MenuIntegerSelector*) p_widget)->SetClipRectangle(p_first, p_second);
}

const MenuWidgetDispatch g_menuIntegerSelectorDispatch =
	{&IntegerUpdateBounds, &SetMenuWidgetFont, &MenuWidgetDerivedNoOp, &IntegerReset, &IntegerInput, &IntegerClip};

MenuIntegerSelector::MenuIntegerSelector() : MenuWidgetBase(-1), m_label(NULL)
{
	m_vtable = &g_menuIntegerSelectorDispatch;
}

// STUB: LEGORACERS2 0x0041b840
LegoS32 MenuIntegerSelector::FormatNumber()
{
	LegoS32 quotient = m_value / 10;
	LegoChar digit = (LegoChar) (m_value % 10);
	if (m_value < 10) {
		m_number[0] = digit + '0';
		m_number[1] = 0;
		return quotient;
	}
	m_number[1] = digit + '0';
	m_number[2] = 0;
	m_number[0] = (LegoChar) (quotient % 10) + '0';
	return quotient / 10;
}

// FUNCTION: LEGORACERS2 0x0041ba00
void MenuIntegerSelector::Reset()
{
	ReleaseChildSprites();
	if (m_label != NULL) {
		m_label->Release();
		m_label = NULL;
	}
	ResetState();
}

// STUB: LEGORACERS2 0x0041ba30
LegoU32 MenuIntegerSelector::HandleInput(LegoU32* p_result, const LegoU32* p_input)
{
	*p_result = 0xffffffff;
	LegoS32 direction = GetNavigationDirection(p_input, 1);
	if (direction != 0) {
		LegoS32 previous = m_value;
		m_value += direction;
		if (m_value < m_minimum) {
			m_value = m_unk0x14 != 0 ? m_maximum : m_minimum;
		}
		else if (m_maximum < m_value) {
			m_value = m_unk0x14 != 0 ? m_minimum : m_maximum;
		}
		if (m_value != previous) {
			FormatNumber();
			m_label->InitializeNarrowText(m_number, 1, 0);
			*p_result = 3;
		}
	}
	return 0;
}

// FUNCTION: LEGORACERS2 0x0041bab0
void MenuIntegerSelector::SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight)
{
	SetChildClipRectangle(p_topLeft, p_bottomRight);
	if (m_label != NULL) {
		m_label->SetClipRectangle(p_topLeft, p_bottomRight);
	}
}

// STUB: LEGORACERS2 0x0041bae0
void MenuIntegerSelector::UpdateBounds()
{
	MenuWidgetBase::UpdateBounds();
	LegoS32 previous = m_value;
	m_value = m_maximum;
	FormatNumber();
	m_label->InitializeNarrowText(m_number, 1, 0);
	LegoFloat left = m_label->m_positionX;
	LegoFloat top = m_label->m_positionY;
	LegoFloat right = left + m_label->m_measuredWidth;
	LegoFloat bottom = top + m_label->GetLineHeight();
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
	m_value = previous;
	FormatNumber();
	m_label->InitializeNarrowText(m_number, 1, 0);
}
