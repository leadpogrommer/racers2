#ifndef LR2_GAMECONTROLLERSESSION_H
#define LR2_GAMECONTROLLERSESSION_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

// A session-state view embedded in GameController's separately owned storage.
// ResetSession initializes the fields through +0x4b; the first two DWORDs
// remain unnamed because their consumers have not yet been recovered.
struct GameControllerSession {
	LegoU32 m_unk0x00;
	LegoU32 m_unk0x04;
	LegoS32 m_mode;         // +0x08
	LegoS32 m_selectedMode; // +0x0c
	LegoS32 m_track;        // +0x10
	LegoU32 m_unk0x14;      // ResetSession sets to 1
	LegoU32 m_unk0x18;      // ResetSession sets to 1
	LegoU32 m_unk0x1c;      // ResetSession sets to 0
	LegoFloat m_unk0x20;    // ResetSession sets to 1.0f
	LegoU32 m_unk0x24;      // ResetSession sets to 0
	LegoU32 m_unk0x28[8];   // ResetSession clears these eight DWORDs
	LegoU32 m_unk0x48;      // ResetSession sets to 0
};
DECOMP_STATIC_ASSERT(offsetof(GameControllerSession, m_unk0x04) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(GameControllerSession, m_mode) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(GameControllerSession, m_selectedMode) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(GameControllerSession, m_track) == 0x10);
DECOMP_STATIC_ASSERT(offsetof(GameControllerSession, m_unk0x14) == 0x14);
DECOMP_STATIC_ASSERT(offsetof(GameControllerSession, m_unk0x20) == 0x20);
DECOMP_STATIC_ASSERT(offsetof(GameControllerSession, m_unk0x28) == 0x28);
DECOMP_STATIC_ASSERT(offsetof(GameControllerSession, m_unk0x48) == 0x48);

extern GameControllerSession* g_gameControllerSession;

inline GameControllerSession* GetGameControllerSession()
{
	return g_gameControllerSession;
}

#endif
