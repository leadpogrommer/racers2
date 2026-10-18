#ifndef LR2_MENUTRACKSELECTOR_H
#define LR2_MENUTRACKSELECTOR_H

#include "menuwidget.h"

#include <stddef.h>

// A ten-row selection control. Its 0xc4 allocation size is established by
// the MenuManager allocation at 0x0041ca64.
struct MenuTrackSelector : MenuWidgetBase {
	undefined m_unk0x38[0x10];
	LegoU32 m_active;                        // +0x48
	LegoS32 m_selection;                     // +0x4c
	LegoFloat m_positionX;                   // +0x50
	LegoFloat m_positionY;                   // +0x54
	LegoU32 m_layoutInitialized;             // +0x58
	GlobalOverlayRecord* m_trackLabels[10];  // +0x5c
	GlobalOverlayRecord* m_trackNumbers[10]; // +0x84
	GlobalOverlayRecord* m_layoutProbe;      // +0xac; released after measuring
	LegoFloat m_leftLabelExtent;             // +0xb0
	LegoFloat m_numberLabelExtent;           // +0xb4
	LegoFloat m_pointerX;                    // +0xb8
	LegoFloat m_pointerY;                    // +0xbc
	LegoS32 m_hoveredRow;                    // +0xc0

	MenuTrackSelector();
	void Initialize(LegoS32 p_initialValue, const LegoFloat* p_position, LegoU32 p_wrapMode);
	void Reset();
	LegoU32 HandleInput(LegoU32* p_result, const LegoU32* p_input);
	void SetActive(LegoU32 p_active);
	void PopulateRows();
	void UpdateRowSelectionVisual(LegoS32 p_row, LegoU32 p_selected);
	void SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight);
	void UpdateBounds();
	void FindHoveredRow(LegoS32* p_row, MenuNavigationState* p_navigation);
	void SetLayoutPosition(const LegoFloat* p_position);
};

DECOMP_SIZE_ASSERT(MenuTrackSelector, 0xc4);
DECOMP_STATIC_ASSERT(offsetof(MenuTrackSelector, m_active) == 0x48);
DECOMP_STATIC_ASSERT(offsetof(MenuTrackSelector, m_selection) == 0x4c);
DECOMP_STATIC_ASSERT(offsetof(MenuTrackSelector, m_trackLabels) == 0x5c);
DECOMP_STATIC_ASSERT(offsetof(MenuTrackSelector, m_trackNumbers) == 0x84);
DECOMP_STATIC_ASSERT(offsetof(MenuTrackSelector, m_layoutProbe) == 0xac);
DECOMP_STATIC_ASSERT(offsetof(MenuTrackSelector, m_hoveredRow) == 0xc0);

extern const MenuWidgetDispatch g_menuTrackSelectorDispatch;
LegoFloat __fastcall MeasureLocalizedTrackLabelWidth(void*, void*, LegoS32 p_mode, GlobalOverlayRecord* p_record);

#endif // LR2_MENUTRACKSELECTOR_H
