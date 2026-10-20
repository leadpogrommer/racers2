#ifndef LR2_MENUTEXTENTRY_H
#define LR2_MENUTEXTENTRY_H

#include "frontend/widgets/menuwidget.h"

struct MenuTextEntry : MenuWidgetBase {
	undefined m_unk0x38[0x10];
	LegoU16 m_text[10];                            // +0x48
	GlobalOverlayRecord* m_textRecords[8];         // +0x5c
	GlobalOverlayRecord* m_textCursorRecords[8];   // +0x7c
	LegoS32 m_textLength;                          // +0x9c
	LegoU16 m_keyboardCharacters[5][10];           // +0xa0
	GlobalOverlayRecord* m_keyboardRecords[5][10]; // +0x104
	GlobalOverlaySprite* m_actionSprites[3];       // +0x1cc
	LegoS32 m_keyboardRowCount;                    // +0x1d8
	LegoS32 m_selectedRow;                         // +0x1dc
	LegoS32 m_selectedColumn;                      // +0x1e0
	LegoU32 m_textAccepted;                        // +0x1e4
	LegoFloat m_pointerX;                          // +0x1e8
	LegoFloat m_pointerY;                          // +0x1ec
	LegoS32 m_pointerRow;                          // +0x1f0
	LegoS32 m_pointerColumn;                       // +0x1f4
	LegoS32 m_lastKeyboardScanCode;                // +0x1f8

	MenuTextEntry();
	void Initialize(LegoS32 p_initialValue, const LegoFloat* p_center, const LegoU16* p_initialText, LegoU32 p_mode);
	void Reset();
	void AppendCharacter(LegoS32 p_row, LegoS32 p_column);
	void RemoveLastCharacter();
	void SetKeyboardSelectionColor(LegoBool32 p_selected);
	void SetTextCursorColor(LegoBool32 p_selected);
	void HandleKeyboardInput(LegoU32* p_result, LegoS32* p_row, LegoS32* p_column);
	LegoU32 HandleInput(LegoU32* p_result, const LegoU32* p_input);
	LegoU16* GetText();
	LegoBool32 FindPointerSelection(LegoS32* p_row, LegoS32* p_column, MenuNavigationState* p_navigation);
	void SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight);
	void UpdateBounds();
};

DECOMP_SIZE_ASSERT(MenuTextEntry, 0x1fc);
DECOMP_STATIC_ASSERT(offsetof(MenuTextEntry, m_text) == 0x48);
DECOMP_STATIC_ASSERT(offsetof(MenuTextEntry, m_keyboardCharacters) == 0xa0);
DECOMP_STATIC_ASSERT(offsetof(MenuTextEntry, m_keyboardRecords) == 0x104);
DECOMP_STATIC_ASSERT(offsetof(MenuTextEntry, m_actionSprites) == 0x1cc);

extern const MenuWidgetDispatch g_menuTextEntryDispatch;

#endif // LR2_MENUTEXTENTRY_H
