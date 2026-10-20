#include "frontend/widgets/menutoggle.h"

static void __fastcall ToggleUpdateBounds(MenuWidgetBase* p_widget, void*)
{
	((MenuToggle*) p_widget)->UpdateBounds();
}
static void __fastcall ToggleReset(MenuWidgetBase* p_widget, void*)
{
	((MenuToggle*) p_widget)->Reset();
}
static LegoU32 __fastcall ToggleInput(MenuWidgetBase* p_widget, void*, LegoU32* p_result, const LegoU32* p_input)
{
	return ((MenuToggle*) p_widget)->HandleInput(p_result, p_input);
}
static void __fastcall ToggleClip(MenuWidgetBase* p_widget, void*, const LegoFloat* p_first, const LegoFloat* p_second)
{
	((MenuToggle*) p_widget)->SetClipRectangle(p_first, p_second);
}

const MenuWidgetDispatch g_menuToggleDispatch =
	{&ToggleUpdateBounds, &SetMenuWidgetFont, &MenuWidgetDerivedNoOp, &ToggleReset, &ToggleInput, &ToggleClip};

MenuToggle::MenuToggle() : MenuWidgetBase(-1), m_label(NULL)
{
	m_vtable = &g_menuToggleDispatch;
}

// FUNCTION: LEGORACERS2 0x0041ae40
void MenuToggle::Reset()
{
	ReleaseChildSprites();
	if (m_label != NULL) {
		m_label->Release();
		m_label = NULL;
	}
	ResetState();
}

// STUB: LEGORACERS2 0x0041ae70
LegoU32 MenuToggle::HandleInput(LegoU32* p_result, const LegoU32* p_input)
{
	*p_result = 0xffffffff;
	LegoS32 direction = GetNavigationDirection(p_input, 1);
	if (direction != 0) {
		LegoU32 previous = m_value;
		m_value = m_unk0x14 == 0 ? direction == -1 : previous != 1;
		if (m_value != previous) {
			m_label->InitializeTextByIndex(m_value == 0, 1, 0);
			*p_result = 3;
		}
	}
	return 0;
}

// STUB: LEGORACERS2 0x0041aee0
void MenuToggle::GetLabelRectangle(LegoFloat* p_topLeft, LegoFloat* p_bottomRight)
{
	p_topLeft[0] = p_topLeft[1] = 1;
	p_bottomRight[0] = p_bottomRight[1] = 0;
	for (LegoS32 i = 0; i < 2; ++i) {
		m_label->InitializeTextByIndex(i == 0, 1, 0);
		LegoFloat topLeft[2], bottomRight[2];
		m_label->GetRectangle(topLeft, bottomRight);
		if (topLeft[0] < p_topLeft[0]) {
			p_topLeft[0] = topLeft[0];
		}
		if (topLeft[1] < p_topLeft[1]) {
			p_topLeft[1] = topLeft[1];
		}
		if (p_bottomRight[0] < bottomRight[0]) {
			p_bottomRight[0] = bottomRight[0];
		}
		if (p_bottomRight[1] < bottomRight[1]) {
			p_bottomRight[1] = bottomRight[1];
		}
	}
	m_label->InitializeTextByIndex(m_value == 0, 1, 0);
}

// FUNCTION: LEGORACERS2 0x0041afd0
void MenuToggle::SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight)
{
	SetChildClipRectangle(p_topLeft, p_bottomRight);
	if (m_label != NULL) {
		m_label->SetClipRectangle(p_topLeft, p_bottomRight);
	}
}

// STUB: LEGORACERS2 0x0041b000
void MenuToggle::UpdateBounds()
{
	MenuWidgetBase::UpdateBounds();
	LegoFloat topLeft[2], bottomRight[2];
	GetLabelRectangle(topLeft, bottomRight);
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
