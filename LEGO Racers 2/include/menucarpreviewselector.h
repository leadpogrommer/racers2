#ifndef LR2_MENU_CAR_PREVIEW_SELECTOR_H
#define LR2_MENU_CAR_PREVIEW_SELECTOR_H

#include "menuwidget.h"

struct MenuCarPreviewSelector : MenuWidgetBase {
	undefined m_secondaryBounds[0x10]; // +0x38
	LegoU32 m_disabled;                // +0x48
	LegoS32 m_choices[80];             // +0x4c
	LegoS32 m_count;                   // +0x18c
	LegoS32 m_selection;               // +0x190
	void* m_camera;                    // +0x194
	LegoS32 m_cameraLease;             // +0x198
	void* m_carDisplay;                // +0x19c
	void* m_ambient;                   // +0x1a0
	LegoS32 m_ambientLease;            // +0x1a4
	void* m_lights[2];                 // +0x1a8
	LegoS32 m_lightLeases[2];          // +0x1b0
	LegoFloat m_size[2];               // +0x1b8
	LegoFloat m_position[2];           // +0x1c0

	MenuCarPreviewSelector();
	void SetFont(BitmapFont* p_font);
	void ReleaseResources();
	void Initialize(
		LegoS32 p_initialValue,
		const LegoFloat* p_position,
		LegoFloat p_scale,
		LegoU32 p_disabled,
		LegoBool32 p_lighting,
		LegoU32 p_wrap
	);
	void AddChoice(LegoS32 p_choice);
	void SelectChoice(LegoS32 p_index);
	LegoU32 HandleInput(LegoU32* p_result, const LegoU32* p_input);
	void Reset();
	void SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight);
	void UpdateBounds();
	LegoS32 GetChoice(LegoS32* p_choice);
	void InitializeLighting();
	void ReleaseLighting();
	void ConfigureViewport(const LegoFloat* p_position, const LegoFloat* p_size);
};

DECOMP_SIZE_ASSERT(MenuCarPreviewSelector, 0x1c8);
DECOMP_STATIC_ASSERT(offsetof(MenuCarPreviewSelector, m_choices) == 0x4c);
DECOMP_STATIC_ASSERT(offsetof(MenuCarPreviewSelector, m_camera) == 0x194);
DECOMP_STATIC_ASSERT(offsetof(MenuCarPreviewSelector, m_carDisplay) == 0x19c);
DECOMP_STATIC_ASSERT(offsetof(MenuCarPreviewSelector, m_size) == 0x1b8);
DECOMP_STATIC_ASSERT(offsetof(MenuCarPreviewSelector, m_position) == 0x1c0);

extern const MenuWidgetDispatch g_menuCarPreviewSelectorDispatch;

#endif
