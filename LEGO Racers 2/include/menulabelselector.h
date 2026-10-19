#ifndef LR2_MENULABELSELECTOR_H
#define LR2_MENULABELSELECTOR_H

#include "menuwidget.h"

struct MenuLabelSelector : MenuWidgetBase {
	LegoFloat m_secondaryBounds[4];
	LegoS32 m_stringIndices[10];
	LegoS32 m_numbers[10];
	LegoS32 m_count;
	LegoS32 m_selection;
	GlobalOverlayRecord* m_label;
	LegoU32 m_unk0xa4;
	LegoFloat m_minimumWidth;
	MenuLabelSelector(); // Inlined into 41c890 in retail.
	void Initialize(
		LegoS32 p_initialValue,
		const LegoFloat* p_position,
		const LegoS32* p_stringIndices,
		LegoS32 p_count,
		LegoS32 p_selection,
		const LegoS32* p_numbers,
		LegoBool32 p_alignLeft,
		LegoFloat p_scale,
		LegoFloat p_minimumWidth,
		LegoU32 p_wrap
	);
	void SelectLabel(LegoS32);
	void Reset();
	LegoU32 HandleInput(LegoU32*, const LegoU32*);
	void SetClipRectangle(const LegoFloat*, const LegoFloat*);
	void UpdateBounds();
};
DECOMP_SIZE_ASSERT(MenuLabelSelector, 0xac);
DECOMP_STATIC_ASSERT(offsetof(MenuLabelSelector, m_numbers) == 0x70);
DECOMP_STATIC_ASSERT(offsetof(MenuLabelSelector, m_count) == 0x98);
extern const MenuWidgetDispatch g_menuLabelSelectorDispatch;

#endif
