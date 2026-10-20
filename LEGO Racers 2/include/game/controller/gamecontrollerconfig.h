#ifndef LR2_GAMECONTROLLERCONFIG_H
#define LR2_GAMECONTROLLERCONFIG_H

#include "decomp.h"
#include "game/controller/gamecontroller.h"
#include "types.h"

#include <stddef.h>

// Prefix at GameController +0x130. The names embedded earlier in this view
// overlap the separately typed name records in gamecontroller.h.
struct GameControllerSettings {
	void SetPlayerName(const GameControllerName* p_name, LegoS32 p_index);
	LegoFloat m_version;
	GameControllerName m_names[2];
	LegoU32 m_defaultWords[25];
	LegoU32 m_unk0x90;
	LegoU32 m_unk0x94;
	LegoFloat m_musicGain;  // 0x98 -> AudioService mixer/digital music
	LegoFloat m_effectGain; // 0x9c -> AudioService source volume pair
	LegoU32 m_audioQuality; // 0xa0
	LegoU32 m_unk0xa4;
	LegoU32 m_unk0xa8;
	LegoU32 m_unk0xac;
	LegoS32 m_language; // 0xb0
};
DECOMP_SIZE_ASSERT(GameControllerSettings, 0xb4);
DECOMP_STATIC_ASSERT(offsetof(GameControllerSettings, m_language) == 0xb0);

extern GameControllerSettings* g_gameControllerSettings;
inline GameControllerSettings* GetGameControllerSettings()
{
	return g_gameControllerSettings;
}
#endif
