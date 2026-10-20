#ifndef LR2_MENUCHOICESELECTOR_H
#define LR2_MENUCHOICESELECTOR_H

#include "frontend/widgets/menuwidget.h"

struct MenuChoiceSelector : MenuWidgetBase {
	undefined m_unk0x38[0x10];
	LegoS32 m_selection;                    // +0x48
	GlobalOverlayRecord* m_optionLabels[5]; // +0x4c
	LegoFloat m_positionX;                  // +0x60
	LegoFloat m_positionY;                  // +0x64
	LegoS32 m_selectedOption;               // +0x68

	MenuChoiceSelector();
	void UpdateBounds();
	void Destroy();
	LegoU32 HandleInput(LegoU32* p_result, const LegoU32* p_input);
	void SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight);
};

DECOMP_SIZE_ASSERT(MenuChoiceSelector, 0x6c);
DECOMP_STATIC_ASSERT(offsetof(MenuChoiceSelector, m_selection) == 0x48);
DECOMP_STATIC_ASSERT(offsetof(MenuChoiceSelector, m_optionLabels) == 0x4c);
DECOMP_STATIC_ASSERT(offsetof(MenuChoiceSelector, m_selectedOption) == 0x68);

extern const MenuWidgetDispatch g_menuChoiceSelectorDispatch;

#endif // LR2_MENUCHOICESELECTOR_H
