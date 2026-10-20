#ifndef LR2_SECONDARYMENUINTERACTION_H
#define LR2_SECONDARYMENUINTERACTION_H
#include "frontend/menu/menumanager.h"
#include "frontend/secondary/secondarymanager.h"
LegoU32 __fastcall UpdateSecondaryMenuManager(
	MenuManager*,
	void*,
	LegoU32,
	LegoU32,
	LegoU32,
	SecondaryManagerMediaState*
);
void __fastcall ReleaseSecondaryObject8c(void*, void*);
void __fastcall MergeSecondaryMediaSlot(SecondaryManagerMediaSlot*, void*, const SecondaryManagerMediaSlot*);
void __fastcall ResetMenuSelectionWidgets(MenuManager*, void*, LegoU32);
LegoBool32 __fastcall IsMenuOptionEnabled(MenuManager*, void*, LegoS32);
void __fastcall HighlightMenuOption(MenuManager*, void*, LegoS32, LegoU32);
#endif
