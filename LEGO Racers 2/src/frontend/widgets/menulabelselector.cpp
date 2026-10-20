#include "frontend/widgets/menulabelselector.h"

#include <stdio.h>

static void __fastcall LabelUpdateBounds(MenuWidgetBase* p_widget, void*)
{
	((MenuLabelSelector*) p_widget)->UpdateBounds();
}
static void __fastcall LabelReset(MenuWidgetBase* p_widget, void*)
{
	((MenuLabelSelector*) p_widget)->Reset();
}
static LegoU32 __fastcall LabelInput(MenuWidgetBase* p_widget, void*, LegoU32* p_result, const LegoU32* p_input)
{
	return ((MenuLabelSelector*) p_widget)->HandleInput(p_result, p_input);
}
static void __fastcall LabelClip(MenuWidgetBase* p_widget, void*, const LegoFloat* p_first, const LegoFloat* p_second)
{
	((MenuLabelSelector*) p_widget)->SetClipRectangle(p_first, p_second);
}

const MenuWidgetDispatch g_menuLabelSelectorDispatch =
	{&LabelUpdateBounds, &SetMenuWidgetFont, &MenuWidgetDerivedNoOp, &LabelReset, &LabelInput, &LabelClip};

MenuLabelSelector::MenuLabelSelector() : MenuWidgetBase(-1), m_count(0), m_selection(0), m_label(NULL)
{
	m_vtable = &g_menuLabelSelectorDispatch;
}

// STUB: LEGORACERS2 0x0041bbc0
void MenuLabelSelector::SelectLabel(LegoS32 p_index)
{
	m_label->InitializeTextByIndex(m_stringIndices[p_index], 0xd, 0);
	LegoS32 number = m_numbers[p_index];
	if (number >= 0) {
		LegoChar text[24] = {0};
		sprintf(text, "%d", number);
		m_label->AppendNarrowText(text, 1);
	}
}

// FUNCTION: LEGORACERS2 0x0041be00
void MenuLabelSelector::Reset()
{
	ReleaseChildSprites();
	if (m_label != NULL) {
		m_label->Release();
		m_label = NULL;
	}
	ResetState();
}

// STUB: LEGORACERS2 0x0041be30
LegoU32 MenuLabelSelector::HandleInput(LegoU32* p_result, const LegoU32* p_input)
{
	*p_result = 0xffffffff;
	LegoS32 direction = GetNavigationDirection(p_input, 1);
	if (direction != 0) {
		LegoS32 previous = m_selection;
		m_selection += direction;
		if (m_selection < 0) {
			m_selection = m_unk0x14 == 0 ? 0 : m_count - 1;
		}
		else if (m_count - 1 < m_selection) {
			m_selection = m_unk0x14 == 0 ? m_count - 1 : 0;
		}
		if (m_selection != previous) {
			SelectLabel(m_selection);
			*p_result = 3;
		}
	}
	return 0;
}

// FUNCTION: LEGORACERS2 0x0041bec0
void MenuLabelSelector::SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight)
{
	SetChildClipRectangle(p_topLeft, p_bottomRight);
	if (m_label != NULL) {
		m_label->SetClipRectangle(p_topLeft, p_bottomRight);
	}
}

// STUB: LEGORACERS2 0x0041bef0
void MenuLabelSelector::UpdateBounds()
{
	MenuWidgetBase::UpdateBounds();
	for (LegoS32 i = 0; i < m_count; ++i) {
		SelectLabel(i);
		LegoFloat left = m_label->m_positionX;
		LegoFloat top = m_label->m_positionY;
		LegoFloat height = m_label->GetLineHeight();
		LegoFloat width = m_label->m_measuredWidth;
		if (width < m_minimumWidth) {
			LegoFloat extra = m_minimumWidth - width;
			left -= extra * 0.5f;
			width = extra + width;
		}
		if (left < m_bounds[0]) {
			m_bounds[0] = left;
		}
		if (top < m_bounds[1]) {
			m_bounds[1] = top;
		}
		if (m_bounds[2] < width + left) {
			m_bounds[2] = width + left;
		}
		if (m_bounds[3] < top + height) {
			m_bounds[3] = top + height;
		}
	}
	SelectLabel(m_selection);
}
