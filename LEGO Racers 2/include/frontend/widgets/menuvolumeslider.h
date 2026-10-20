#ifndef LR2_MENUVOLUMESLIDER_H
#define LR2_MENUVOLUMESLIDER_H

#include "frontend/widgets/menuwidget.h"

struct MenuVolumeSlider : MenuWidgetBase {
	undefined m_unk0x38[0x10];
	LegoFloat m_value;                 // +0x48
	LegoFloat m_step;                  // +0x4c
	LegoU32 m_audioTargets;            // +0x50
	GlobalOverlayRecord* m_valueLabel; // +0x54
	LegoFloat m_originX;               // +0x58
	LegoFloat m_originY;               // +0x5c
	GlobalOverlaySprite* m_sprites[4]; // +0x60

	MenuVolumeSlider();
	void UpdateBounds();
	void Destroy();
	LegoU32 HandleInput(LegoU32* p_result, const LegoU32* p_input);
	void UpdateMarker();
	void SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight);
};

DECOMP_SIZE_ASSERT(MenuVolumeSlider, 0x70);
DECOMP_STATIC_ASSERT(offsetof(MenuVolumeSlider, m_value) == 0x48);
DECOMP_STATIC_ASSERT(offsetof(MenuVolumeSlider, m_valueLabel) == 0x54);
DECOMP_STATIC_ASSERT(offsetof(MenuVolumeSlider, m_originX) == 0x58);
DECOMP_STATIC_ASSERT(offsetof(MenuVolumeSlider, m_sprites) == 0x60);

extern const MenuWidgetDispatch g_menuVolumeSliderDispatch;

#endif // LR2_MENUVOLUMESLIDER_H
