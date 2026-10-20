#ifndef LR2_MENUTEXTGROUP_H
#define LR2_MENUTEXTGROUP_H

#include "frontend/widgets/menuwidget.h"

struct MenuTextGroup : MenuWidgetBase {
	undefined m_unk0x38[0x10];
	GlobalOverlayRecord* m_records[6]; // +0x48

	MenuTextGroup();
	void UpdateBounds();
	void Destroy();
	void SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight);
};

DECOMP_SIZE_ASSERT(MenuTextGroup, 0x60);
DECOMP_STATIC_ASSERT(offsetof(MenuTextGroup, m_records) == 0x48);

extern const MenuWidgetDispatch g_menuTextGroupDispatch;

#endif // LR2_MENUTEXTGROUP_H
