#ifndef LR2_MENUPANELSET_H
#define LR2_MENUPANELSET_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

struct MenuPanel;

// Owned by SecondaryManager +0x84. The six pointer words share the retail
// zero-initializer used by CharBuffer24; this object is not a text buffer.
struct MenuPanelSet {
	MenuPanel* m_panels[6];
	LegoU32 m_active;
	LegoU32 m_resourceMode;
	LegoS32 m_unk0x20;
	LegoS32 m_unk0x24;

	void Initialize();
	void Release();
	void Begin(LegoU32 p_mode);
	void End();
	void SetActive(LegoU32 p_active);
};
DECOMP_SIZE_ASSERT(MenuPanelSet, 0x28);
DECOMP_STATIC_ASSERT(offsetof(MenuPanelSet, m_active) == 0x18);
DECOMP_STATIC_ASSERT(offsetof(MenuPanelSet, m_resourceMode) == 0x1c);

#endif
