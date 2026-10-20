#include "game/session/gamecontrollersession.h"

#include "game/controller/gamecontroller.h"

extern LegoU32 g_gameControllerStorage[sizeof(GameController) / sizeof(LegoU32)];

// GLOBAL: LEGORACERS2 0x0053e3c0
GameControllerSession* g_gameControllerSession = (GameControllerSession*) ((LegoU8*) g_gameControllerStorage + 0x80ec);
