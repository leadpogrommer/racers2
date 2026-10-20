#ifndef LR2_SECONDARYSAVEDGAME_H
#define LR2_SECONDARYSAVEDGAME_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

struct SecondarySavedGame {
	LegoU8 m_settings[0x7fb4];
	LegoU32 m_cookie;
	LegoU32 m_settingsChecksum;
	LegoU8 m_session[0x1e30];
	LegoU32 m_sessionChecksum;
};
DECOMP_SIZE_ASSERT(SecondarySavedGame, 0x9df0);
DECOMP_STATIC_ASSERT(offsetof(SecondarySavedGame, m_cookie) == 0x7fb4);
DECOMP_STATIC_ASSERT(offsetof(SecondarySavedGame, m_session) == 0x7fbc);
extern SecondarySavedGame g_secondarySavedGame;
void ResetSecondarySavedGameBuffer();

#endif
