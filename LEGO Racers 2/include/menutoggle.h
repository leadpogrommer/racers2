#ifndef LR2_MENUTOGGLE_H
#define LR2_MENUTOGGLE_H

#include "menuwidget.h"

struct MenuToggle : MenuWidgetBase {
	LegoFloat m_secondaryBounds[4];
	LegoU32 m_value;
	GlobalOverlayRecord* m_label;
	MenuToggle(); // Inlined into 41c890 in retail.
	void Reset();
	LegoU32 HandleInput(LegoU32*, const LegoU32*);
	void GetLabelRectangle(LegoFloat*, LegoFloat*);
	void SetClipRectangle(const LegoFloat*, const LegoFloat*);
	void UpdateBounds();
};
DECOMP_SIZE_ASSERT(MenuToggle, 0x50);
DECOMP_STATIC_ASSERT(offsetof(MenuToggle, m_label) == 0x4c);
extern const MenuWidgetDispatch g_menuToggleDispatch;

#endif
