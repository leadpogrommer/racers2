#ifndef LR2_MENUCHARACTERPREVIEWSELECTOR_H
#define LR2_MENUCHARACTERPREVIEWSELECTOR_H

#include "menuwidget.h"

struct MenuCharacterPreviewChoice {
	LegoS32 m_parts[4]; // head model, face, torso texture, leg texture
};

// Working name: the control switches four component indices on the shared
// character preview object. The original component names are unproven.
struct MenuCharacterPreviewSelector : MenuWidgetBase {
	LegoFloat m_secondaryBounds[4];           // +0x38
	LegoU32 m_disabled;                       // +0x48
	MenuCharacterPreviewChoice m_choices[10]; // +0x4c
	LegoS32 m_count;                          // +0xec
	LegoS32 m_selection;                      // +0xf0
	void* m_camera;                           // +0xf4
	LegoS32 m_cameraLease;                    // +0xf8
	void* m_previewObject;                    // +0xfc
	LegoS32 m_previewLease;                   // +0x100
	void* m_ambient;                          // +0x104
	LegoS32 m_ambientLease;                   // +0x108
	void* m_lights[2];                        // +0x10c
	LegoS32 m_lightLeases[2];                 // +0x114
	LegoFloat m_size[2];                      // +0x11c
	LegoFloat m_position[2];                  // +0x124

	MenuCharacterPreviewSelector();
	void ReleasePreviewResources();
	void AddChoice(const MenuCharacterPreviewChoice*);
	void SelectChoice(LegoS32);
	LegoU32 HandleInput(LegoU32*, const LegoU32*);
	void Reset();
	void ApplyChoice(LegoS32);
	void SetClipRectangle(const LegoFloat*, const LegoFloat*);
	void UpdateBounds();
	LegoS32 GetChoice(MenuCharacterPreviewChoice*);
	void InitializeLighting();
	void ReleaseLighting();
	void ConfigureViewport(const LegoFloat*, const LegoFloat*);
};

DECOMP_SIZE_ASSERT(MenuCharacterPreviewChoice, 0x10);
DECOMP_SIZE_ASSERT(MenuCharacterPreviewSelector, 0x12c);
DECOMP_STATIC_ASSERT(offsetof(MenuCharacterPreviewSelector, m_choices) == 0x4c);
DECOMP_STATIC_ASSERT(offsetof(MenuCharacterPreviewSelector, m_count) == 0xec);
DECOMP_STATIC_ASSERT(offsetof(MenuCharacterPreviewSelector, m_camera) == 0xf4);
DECOMP_STATIC_ASSERT(offsetof(MenuCharacterPreviewSelector, m_previewObject) == 0xfc);
DECOMP_STATIC_ASSERT(offsetof(MenuCharacterPreviewSelector, m_lightLeases) == 0x114);
DECOMP_STATIC_ASSERT(offsetof(MenuCharacterPreviewSelector, m_position) == 0x124);

extern const MenuWidgetDispatch g_menuCharacterPreviewSelectorDispatch;

#endif
