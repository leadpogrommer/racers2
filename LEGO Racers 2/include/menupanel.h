#ifndef LR2_MENUPANEL_H
#define LR2_MENUPANEL_H

#include "globaloverlaymanager.h"

struct MenuPanel {
	LegoU32 m_listId;
	GlobalOverlaySprite* m_sprites[9];
	GlobalOverlaySprite* m_shadows[9];
	LegoS32 m_contextType; // 0x4c
	void* m_context;
	LegoU32 m_transition;
	LegoFloat m_velocity[2];
	LegoFloat m_remaining;
	LegoS32 m_selection;
	LegoS32 m_style;
	LegoS32 m_targetStyle;
	LegoFloat m_position[2];
	LegoFloat m_targetPosition[2];
	LegoFloat m_size[2];
	LegoFloat m_targetSize[2];
	LegoU32 m_translucent;
	LegoU32 m_targetTranslucent;
	GlobalOverlaySprite* m_secondarySprites[9]; // 0x98
	GlobalOverlaySprite* m_secondaryShadows[9]; // 0xbc
	LegoU32 m_hasSecondary;
	LegoU32 m_targetHasSecondary;
	LegoFloat m_secondaryPosition[2];
	LegoFloat m_targetSecondaryPosition[2];
	LegoFloat m_secondarySize[2];
	LegoFloat m_targetSecondarySize[2];

	MenuPanel();
	void Initialize();
	void Release();
	void Activate(LegoS32 p_contextType, void* p_context);
	void AllocateSprites(LegoU32 p_primary, LegoU32 p_secondary);
	void ReleaseSprites();
	void BeginClosing();
	void Deactivate();
	void Update(LegoFloat p_elapsed);
	void UpdateClipping();
	void ResetGeometry();
	void SetTransitionVelocity(LegoU32 p_transition);
	void UpdateSpriteGeometry(LegoS32 p_style, const LegoFloat*, const LegoFloat*, LegoU32 p_secondary);
	LegoS32 QuerySelection(LegoS32* p_style, LegoU32* p_hasSecondary);
	LegoBool32 QueryTargetGeometry(LegoS32, LegoFloat*, LegoFloat*, LegoFloat*, LegoFloat*, LegoU32*);
};
DECOMP_SIZE_ASSERT(MenuPanel, 0x108);
DECOMP_STATIC_ASSERT(offsetof(MenuPanel, m_contextType) == 0x4c);
DECOMP_STATIC_ASSERT(offsetof(MenuPanel, m_size) == 0x80);
DECOMP_STATIC_ASSERT(offsetof(MenuPanel, m_secondarySprites) == 0x98);
DECOMP_STATIC_ASSERT(offsetof(MenuPanel, m_secondarySize) == 0xf8);

struct MenuPanelTuning {
	LegoFloat m_padding[2]; // 561160
	LegoFloat m_largePadding[2];
	LegoFloat m_closeDuration;
	LegoFloat m_unk0x14;
	LegoFloat m_secondaryBorder[2];
	LegoFloat m_secondaryOffset[2];
	LegoFloat m_closeEnd;
	LegoFloat m_resizeDuration;
	LegoFloat m_shadowOffset[2];
	LegoFloat m_smallBorder[2];
	LegoFloat m_openStart;
	LegoFloat m_unk0x44;
	LegoFloat m_secondaryInset[2];
	LegoFloat m_openEnd;
	LegoFloat m_unk0x54;
	LegoFloat m_largeBorder[2];
	LegoFloat m_unk0x60[2];
	LegoFloat m_smallPadding[2];
	LegoFloat m_duration;
	LegoFloat m_unk0x74;
	LegoFloat m_border[2];
};
DECOMP_SIZE_ASSERT(MenuPanelTuning, 0x80);
extern MenuPanelTuning g_menuPanelTuning;

#endif
