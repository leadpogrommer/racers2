#ifndef LR2_MENUINPUTBINDINGS_H
#define LR2_MENUINPUTBINDINGS_H

#include "frontend/widgets/menuwidget.h"

struct MenuLabelSelector;

// Input bindings are copied into this view by the menu initializer. The
// meaning of individual words in the copied block is still being recovered.
struct MenuInputBindingWidget : MenuWidgetBase {
	undefined m_gap0x38[0x10];
	LegoFloat m_positionX; // +0x48
	LegoFloat m_positionY; // +0x4c
	LegoFloat m_unk0x50;
	LegoFloat m_unk0x54;
	LegoU32 m_bindingData[43];              // +0x58..+0x103, copied from caller
	void* m_bindingSource;                  // +0x104
	LegoS32 m_bindingMode;                  // +0x108
	GlobalOverlayRecord* m_actionRows[15];  // +0x10c
	LegoS32 m_selectedAction;               // +0x148
	GlobalOverlayRecord* m_inputRows[11];   // +0x14c
	LegoS32 m_pendingAction;                // +0x178
	MenuLabelSelector* m_primarySelector;   // +0x17c
	MenuLabelSelector* m_secondarySelector; // +0x180
	LegoFloat m_unk0x184;
	LegoU32 m_inputLatch; // +0x188

	MenuInputBindingWidget();
	void SetFontAndCreateSelectors(BitmapFont* p_font);
	void DestroySelectors();
	void Initialize(
		LegoS32 p_initialValue,
		const LegoFloat* p_position,
		LegoS32 p_bindingMode,
		const LegoU32* p_bindingData,
		void* p_bindingSource,
		LegoU32 p_wrapMode
	);
	void Reset();
	LegoU32 HandleInput(LegoU32* p_result, const LegoU32* p_input);
	void SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight);
	void UpdateBounds();
	void RefreshRows();
	void SetActionRowSelected(LegoS32 p_index, LegoBool32 p_selected);
};

DECOMP_SIZE_ASSERT(MenuInputBindingWidget, 0x18c);
DECOMP_STATIC_ASSERT(offsetof(MenuInputBindingWidget, m_bindingData) == 0x58);
DECOMP_STATIC_ASSERT(offsetof(MenuInputBindingWidget, m_actionRows) == 0x10c);
DECOMP_STATIC_ASSERT(offsetof(MenuInputBindingWidget, m_inputRows) == 0x14c);
DECOMP_STATIC_ASSERT(offsetof(MenuInputBindingWidget, m_inputLatch) == 0x188);

extern const MenuWidgetDispatch g_menuInputBindingWidgetDispatch;

#endif // LR2_MENUINPUTBINDINGS_H
