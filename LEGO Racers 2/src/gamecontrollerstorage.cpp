#include "gamecontroller.h"

// GLOBAL: LEGORACERS2 0x005542c0
GameObjectTemplate g_gameControllerTemplate;

// GLOBAL: LEGORACERS2 0x005543f8
LegoU32 g_gameControllerStorage[sizeof(GameController) / sizeof(LegoU32)];
GameController& g_gameController = *(GameController*) g_gameControllerStorage;
