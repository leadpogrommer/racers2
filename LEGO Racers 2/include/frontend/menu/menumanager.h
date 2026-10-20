#ifndef LR2_MENUMANAGER_H
#define LR2_MENUMANAGER_H

#include "game/objects/gamemanager.h"

#include <stddef.h>

struct MenuWidgetBase;
struct MenuChoiceSelector;
struct GlobalOverlaySprite;

struct MenuManagerDispatchView {
	GameObjectDispatchView m_gameObject;
	LegoU32(__fastcall* m_getAllocationSize)(GameObject*, void*);
};
DECOMP_SIZE_ASSERT(MenuManagerDispatchView, 0x54);
DECOMP_STATIC_ASSERT(offsetof(MenuManagerDispatchView, m_getAllocationSize) == 0x50);

struct MenuManager : GameObject {
	undefined m_unk0x58[0x140];
	LegoU32 m_unk0x198;
	LegoU32 m_unk0x19c;
	LegoU32 m_unk0x1a0;
	LegoU32 m_unk0x1a4;
	LegoU32 m_unk0x1a8;
	undefined m_unk0x1ac[0x1b8 - 0x1ac];
	LegoFloat m_panelTopLeft[2];         // 0x1b8
	LegoFloat m_panelBottomRight[2];     // 0x1c0
	LegoFloat m_secondaryTopLeft[2];     // 0x1c8
	LegoFloat m_secondaryBottomRight[2]; // 0x1d0
	LegoU32 m_unk0x1d8;
	LegoU32 m_unk0x1dc;
	LegoU32 m_unk0x1e0;
	undefined m_unk0x1e4[4];
	LegoU32 m_unk0x1e8;
	LegoU32 m_unk0x1ec;
	undefined m_unk0x1f0[0x14];
	LegoU32 m_unk0x204;
	LegoU32 m_unk0x208;
	MenuChoiceSelector* m_extraChoiceSelector; // 0x20c
	LegoU32 m_unk0x210;
	LegoU32 m_unk0x214;
	LegoU32 m_unk0x218;
	LegoU32 m_unk0x21c;
	LegoU32 m_unk0x220;
	LegoU32 m_unk0x224;
	LegoU32 m_unk0x228;
	LegoU32 m_unk0x22c;
	void* m_overlayRecords[14];
	GlobalOverlaySprite* m_transitionCover; // 0x268
	MenuWidgetBase* m_widgets[18];          // 0x26c

	MenuManager(GameObjectTemplate* p_template);
	~MenuManager();
	void Initialize();
	void Remove();
	void ApplyFonts();
	LegoBool32 GetPanelRectangle(LegoFloat*, LegoFloat*);
	void GetSecondaryPanelRectangle(LegoFloat*, LegoFloat*);
	void SetPanelClipRectangle(const LegoFloat*, const LegoFloat*);
	void SetSecondaryPanelClipRectangle(const LegoFloat*, const LegoFloat*);
};

DECOMP_SIZE_ASSERT(MenuManager, 0x2b4);
DECOMP_STATIC_ASSERT(offsetof(MenuManager, m_unk0x198) == 0x198);
DECOMP_STATIC_ASSERT(offsetof(MenuManager, m_overlayRecords) == 0x230);
DECOMP_STATIC_ASSERT(offsetof(MenuManager, m_transitionCover) == 0x268);
DECOMP_STATIC_ASSERT(offsetof(MenuManager, m_widgets) == 0x26c);

extern const MenuManagerDispatchView g_menuManagerDispatch;

#endif // LR2_MENUMANAGER_H
