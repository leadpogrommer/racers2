#ifndef LR2_SCREENPRINTER_H
#define LR2_SCREENPRINTER_H

#include "game/objects/gamemanager.h"

struct GlobalOverlayRecord;
struct ScreenPrinter : GameObject {
	ScreenPrinter(GameObjectTemplate* p_template);
	void Initialize();
	void Remove();
	LegoU32 Update(LegoU32 p_elapsed);
	void ApplyFont();
	void SetText(const LegoChar* p_text, LegoU32 p_index);
	void ClearText(LegoU32 p_index);
	GlobalOverlayRecord* m_records[4];
};
DECOMP_SIZE_ASSERT(ScreenPrinter, 0x68);

struct ScreenPrinterDispatch {
	GameObjectDispatchView m_base;
	LegoU32(__fastcall* m_size)(GameObject*, void*);
};
extern const ScreenPrinterDispatch g_screenPrinterDispatch;
GameObject* __fastcall ConstructScreenPrinter(void*, void*, GameObjectTemplate*);

#endif
