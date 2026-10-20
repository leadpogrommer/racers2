#ifndef LR2_GAMERANDOM_H
#define LR2_GAMERANDOM_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

struct GameFloatRandom {
	LegoU32 m_multiplier;
	LegoU32 m_increment;
	LegoU32 m_state;
	LegoU32 m_mantissaMask;
	LegoU32 m_frame;
	GameFloatRandom* Initialize();
};
struct GameIntegerRandom {
	LegoU32 m_multiplier;
	LegoU32 m_increment;
	LegoU32 m_state;
	GameIntegerRandom* Initialize();
};
struct GameRandomState {
	GameFloatRandom m_floatStreams[2];
	GameIntegerRandom m_integerStreams[2];
	LegoU32 m_checkpoint0;
	LegoU32 m_checkpoint1;
	LegoU32 m_unk0x48;
};
DECOMP_SIZE_ASSERT(GameFloatRandom, 0x14);
DECOMP_SIZE_ASSERT(GameIntegerRandom, 0x0c);
DECOMP_SIZE_ASSERT(GameRandomState, 0x4c);
DECOMP_STATIC_ASSERT(offsetof(GameRandomState, m_integerStreams) == 0x28);
DECOMP_STATIC_ASSERT(offsetof(GameRandomState, m_checkpoint0) == 0x40);

GameRandomState* GetGameRandomState();

#endif
