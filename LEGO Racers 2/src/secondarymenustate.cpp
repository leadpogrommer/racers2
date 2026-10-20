#include "gamecontroller.h"
#include "gamecontrollersession.h"
#include "gamecontrollersessionqueries.h"
#include "secondarymanager.h"
#include "secondarysavedgame.h"

#include <string.h>

LegoU32 __fastcall GetFoyerModeProgressValue(const LegoU32*, void*, LegoU32);
void __fastcall CaptureSecondarySessionData(void*, void*);

// STUB: LEGORACERS2 0x004114f0
void __fastcall IncrementMenuModeProgress(void* p_state, void*, LegoU32 p_mode)
{
	LegoU32* progress = (LegoU32*) ((LegoU8*) p_state + 0x1dfc);
	if ((LegoS32) GetFoyerModeProgressValue(progress, NULL, p_mode) < 10) {
		IncrementSecondaryModeProgress(progress, NULL, p_mode);
	}
}

// STUB: LEGORACERS2 0x00411740
void __fastcall CaptureSecondarySessionData(void* p_state, void*)
{
	memcpy((LegoU8*) p_state + 0x1e34, p_state, 0x1e34);
}

// STUB: LEGORACERS2 0x00411e90
void __fastcall SetMenuExitAction(void* p_state, void*, LegoU32 p_action)
{
	if ((LegoS32) p_action >= 9 && (LegoS32) p_action <= 12) {
		*(LegoU32*) ((LegoU8*) p_state + 0x1dec) = p_action;
	}
}

// STUB: LEGORACERS2 0x00429370
void __fastcall BeginSecondarySave(void* p_object, void*)
{
	SecondaryManager* manager = (SecondaryManager*) p_object;
	manager->m_state = 4;
	manager->m_unk0x5c = 1;
	if (GetGameControllerSession()->m_unk0x04 != 0) {
		CaptureSecondarySessionData(g_gameController.m_unk0x8c, NULL);
	}
	ResetSecondarySavedGameBuffer();
}

// STUB: LEGORACERS2 0x004293b0
void __fastcall BeginSecondaryLoad(void* p_object, void*)
{
	SecondaryManager* manager = (SecondaryManager*) p_object;
	manager->m_state = 3;
	manager->m_unk0x5c = 1;
	ResetSecondarySavedGameBuffer();
}

// STUB: LEGORACERS2 0x004293f0
void __fastcall SetSecondarySaveSlot(void* p_object, void*, LegoU32 p_slot)
{
	((SecondaryManager*) p_object)->m_unk0x240 = p_slot;
}
