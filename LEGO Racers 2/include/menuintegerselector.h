#ifndef LR2_MENUINTEGERSELECTOR_H
#define LR2_MENUINTEGERSELECTOR_H

#include "menuwidget.h"

struct MenuIntegerSelector : MenuWidgetBase {
	LegoFloat m_secondaryBounds[4];
	LegoS32 m_minimum;
	LegoS32 m_maximum;
	LegoS32 m_value;
	LegoChar m_number[4];
	GlobalOverlayRecord* m_label;
	MenuIntegerSelector(); // Inlined into 41c890 in retail.
	LegoS32 FormatNumber();
	void Reset();
	LegoU32 HandleInput(LegoU32*, const LegoU32*);
	void SetClipRectangle(const LegoFloat*, const LegoFloat*);
	void UpdateBounds();
};
DECOMP_SIZE_ASSERT(MenuIntegerSelector, 0x5c);
DECOMP_STATIC_ASSERT(offsetof(MenuIntegerSelector, m_number) == 0x54);
extern const MenuWidgetDispatch g_menuIntegerSelectorDispatch;

#endif
