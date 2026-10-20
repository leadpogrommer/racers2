#ifndef LR2_SECONDARYDEFERRED_H
#define LR2_SECONDARYDEFERRED_H
#include "secondarymanager.h"
// Explicit unfinished post-menu operations. See secondarydeferred.cpp.
LegoS32 __fastcall UpdateSecondaryCharacterSelection(void*, void*, LegoU32, SecondaryManagerMediaState*);
void __fastcall BeginSecondaryCharacterSelection(void*, void*, LegoU32);
LegoS32 __fastcall UpdateSecondaryCarSelection(void*, void*, LegoU32, SecondaryManagerMediaState*);
void __fastcall FinishSecondaryCarSelection(void*, void*);
void __fastcall BeginSecondaryCarSelection(void*, void*, LegoU32, LegoS32, LegoU32);
void __fastcall RestartSecondaryBattle(void*, void*);
void __fastcall RestartSecondaryRace(void*, void*, LegoU32);
void __fastcall StartSecondarySessionRace(void*, void*, LegoU32, LegoU32, LegoU32);
void __fastcall SendSecondaryNetworkRestart(void*, void*);
void __fastcall SendSecondaryNetworkNextRace(void*, void*);
void __fastcall SendSecondaryNetworkContinue(void*, void*);
void __fastcall RequestSecondaryNetworkExit(void*, void*);
#endif
