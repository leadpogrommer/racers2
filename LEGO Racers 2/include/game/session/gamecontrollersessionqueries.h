#ifndef LR2_GAMECONTROLLERSESSIONQUERIES_H
#define LR2_GAMECONTROLLERSESSIONQUERIES_H
#include "game/controller/gamecontroller.h"
GameControllerItemBlock* __fastcall GetSecondarySessionCar(void*, void*, LegoU32, LegoU32);
const LegoU32* __fastcall GetSecondarySessionCharacter(void*, void*, LegoU32, LegoU32*);
GameControllerScore* __fastcall GetControllerTrackScore(void*, void*, LegoU32, LegoU32);
GameControllerScore* __fastcall GetControllerWorldScore(void*, void*, LegoU32, LegoU32);
void __fastcall ApplySecondarySessionData(void*, void*);
void __fastcall IncrementSecondaryModeProgress(LegoU32*, void*, LegoU32);
LegoBool32 __fastcall IsControllerBodyInputEnabled(void*, void*);
#endif
