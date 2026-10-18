#ifndef LR2_MENUWIDGET_H
#define LR2_MENUWIDGET_H

#include "decomp.h"
#include "globaloverlaymanager.h"
#include "types.h"

#include <stddef.h>

class BitmapFont;

struct MenuWidgetBase;

struct GameController;

// A navigation record shared by the secondary manager's menu widgets. Only
// the fields read or written by these helpers are established here.
struct MenuNavigationState {
	LegoU32 m_active;
	struct MenuPointerState* m_pointer;
	LegoU32 m_horizontalFlags;
	LegoU32 m_verticalFlags;

	void GetPointerPosition(LegoFloat* p_position) const;
	void ReadDirectionInput(LegoU32 p_direction);
	LegoBool32 IsSpriteUnderPointer(const GlobalOverlaySprite* p_sprite) const;
	LegoBool32 IsRecordUnderPointer(const GlobalOverlayRecord* p_record) const;
};

struct MenuPointerState {
	LegoU32 m_unk0x00;
	LegoFloat m_x;
	LegoFloat m_y;
	undefined m_unk0x0c[0x24];
	LegoU32 m_directionInput[5]; // +0x30, copied by ReadDirectionInput
};

DECOMP_STATIC_ASSERT(offsetof(MenuPointerState, m_x) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(MenuPointerState, m_directionInput) == 0x30);

struct MenuWidgetDispatch {
	typedef void(__fastcall* Action)(MenuWidgetBase*, void*);
	typedef void(__fastcall* SetFont)(MenuWidgetBase*, void*, BitmapFont*);
	typedef LegoU32(__fastcall* ReturnZeroTwoArguments)(MenuWidgetBase*, void*, LegoU32, LegoU32);
	typedef void(__fastcall* SetClipRectangle)(MenuWidgetBase*, void*, const LegoFloat*, const LegoFloat*);

	Action m_updateBounds;                     // +0x00
	SetFont m_setFont;                         // +0x04
	Action m_unk0x08;                          // +0x08, original target is a bare RET
	Action m_reset;                            // +0x0c
	ReturnZeroTwoArguments m_unk0x10;          // +0x10
	SetClipRectangle m_setClipRectangle;       // +0x14
};

struct MenuWidgetBase {
	const MenuWidgetDispatch* m_vtable;
	GlobalOverlaySprite* m_childSprites[2]; // +0x04, +0x08
	LegoU32 m_state;                         // +0x0c
	LegoS32 m_initialValue;                  // +0x10
	LegoU32 m_unk0x14;
	BitmapFont* m_font;                      // +0x18
	LegoU32 m_unk0x1c;
	LegoU32 m_unk0x20;
	LegoU32 m_unk0x24;
	LegoFloat m_bounds[4];                   // +0x28: left, top, right, bottom

	MenuWidgetBase(LegoS32 p_initialValue);
	void SetFont(BitmapFont* p_font);
	void RestoreBaseDispatch();
	void ResetState();
	void ReleaseChildSprites();
	void SetChildClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight);
	void UpdateBounds();
	LegoS32 GetNavigationDirection(const LegoU32* p_input, LegoU32 p_axis);
};

DECOMP_SIZE_ASSERT(MenuWidgetBase, 0x38);
DECOMP_STATIC_ASSERT(offsetof(MenuWidgetBase, m_childSprites) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(MenuWidgetBase, m_font) == 0x18);
DECOMP_STATIC_ASSERT(offsetof(MenuWidgetBase, m_bounds) == 0x28);
DECOMP_SIZE_ASSERT(MenuWidgetDispatch, 0x18);

extern const MenuWidgetDispatch g_menuWidgetBaseDispatch;
extern LegoFloat g_menuPointerOffsetX;
extern LegoFloat g_menuPointerOffsetY;
void InitializeMenuPointerOffsets();

// 0x00416b80 is the shared derived-widget font callback. The base table uses
// MenuWidgetBase::SetFont at 0x00419720 instead.
void __fastcall SetMenuWidgetFont(MenuWidgetBase* p_widget, void*, BitmapFont* p_font);

#endif // LR2_MENUWIDGET_H
