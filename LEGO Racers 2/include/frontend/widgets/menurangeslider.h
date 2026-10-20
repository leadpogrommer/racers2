#ifndef LR2_MENURANGESLIDER_H
#define LR2_MENURANGESLIDER_H

#include "core/strings/charbuffer24.h"
#include "frontend/widgets/menuwidget.h"

struct MenuRangeSlider : MenuWidgetBase {
	LegoFloat m_secondaryBounds[4];
	LegoFloat m_centerX;
	LegoFloat m_centerY;
	LegoFloat m_width;
	LegoFloat m_value;
	LegoFloat m_step;
	LegoFloat m_displayedValue;
	LegoFloat m_displayStep;
	LegoU32 m_dragging;
	CharBuffer24* m_valueNames;
	GlobalOverlayRecord* m_labels[3];
	GlobalOverlaySprite* m_sprites[4];

	MenuRangeSlider();
	void Reset();
	void UpdateMarker();
	LegoU32 HandleInput(LegoU32*, const LegoU32*);
	void SetClipRectangle(const LegoFloat*, const LegoFloat*);
	void UpdateBounds();
	LegoFloat GetPointerValue() const;
	LegoS32 SnapValue(LegoFloat, LegoFloat*) const;
	void UpdateValueLabel();
};
DECOMP_SIZE_ASSERT(MenuRangeSlider, 0x88);
DECOMP_STATIC_ASSERT(offsetof(MenuRangeSlider, m_value) == 0x54);
DECOMP_STATIC_ASSERT(offsetof(MenuRangeSlider, m_valueNames) == 0x68);
DECOMP_STATIC_ASSERT(offsetof(MenuRangeSlider, m_sprites) == 0x78);
extern const MenuWidgetDispatch g_menuRangeSliderDispatch;

#endif
