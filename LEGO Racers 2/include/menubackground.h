#ifndef LR2_MENUBACKGROUND_H
#define LR2_MENUBACKGROUND_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

struct GlobalOverlaySprite;
struct ControllerEventRecordTable;

struct MenuBackground {
	GlobalOverlaySprite* m_sprites[8]; // 0x00, current then previous quadrants
	LegoS32 m_current;                 // 0x20
	LegoS32 m_requested;               // 0x24
	LegoS32 m_previous;                // 0x28
	LegoU32 m_mode;                    // 0x2c, 2 chooses from menu state
	LegoU32 m_transition;              // 0x30
	LegoFloat m_remaining;             // 0x34
	LegoU32 m_loaded;                  // 0x38

	MenuBackground();
	~MenuBackground();
	void Initialize();
	void Update(LegoFloat p_elapsed);
	void Load();
	void Release();
	void SetMode(LegoU32 p_mode);
	void Hide();
	LegoS32 GetMenuCategory();
};
DECOMP_SIZE_ASSERT(MenuBackground, 0x3c);
DECOMP_STATIC_ASSERT(offsetof(MenuBackground, m_current) == 0x20);
DECOMP_STATIC_ASSERT(offsetof(MenuBackground, m_remaining) == 0x34);
DECOMP_STATIC_ASSERT(offsetof(MenuBackground, m_loaded) == 0x38);

extern LegoFloat g_menuBackgroundFadeDuration;
void __fastcall LoadControllerBackgroundTextures(ControllerEventRecordTable*, void*);
void __fastcall ReleaseControllerBackgroundTextures(ControllerEventRecordTable*, void*);

#endif
