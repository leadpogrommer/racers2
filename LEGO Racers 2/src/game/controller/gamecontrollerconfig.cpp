#include "game/controller/gamecontrollerconfig.h"

#include "game/controller/gamecontroller.h"

extern LegoU32 g_gameControllerStorage[sizeof(GameController) / sizeof(LegoU32)];

// GLOBAL: LEGORACERS2 0x0053e3bc
GameControllerSettings* g_gameControllerSettings =
	(GameControllerSettings*) ((LegoU8*) g_gameControllerStorage + 0x130);
