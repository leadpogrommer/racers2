#ifndef LR2_SECONDARYMENUACTIONS_H
#define LR2_SECONDARYMENUACTIONS_H
#include "menumanager.h"
void __fastcall DisableSecondaryInactivityTimer(void*, void*);
void __fastcall RestoreMenuManagerAction(MenuManager*, void*, LegoU32, LegoS32);
LegoBool32 __fastcall ApplyMenuConfirmAction(MenuManager*, void*);
LegoS32 __fastcall GetMenuParentAction(MenuManager*, void*);
LegoS32 __fastcall GetSecondaryPendingMenuAction(MenuManager*, void*);
LegoBool32 __fastcall GetMenuEnteredName(MenuManager*, void*, LegoS32, LegoU16**);
LegoBool32 __fastcall CommitMenuInputBindings(MenuManager*, void*, LegoS32, LegoU32*);
#endif
