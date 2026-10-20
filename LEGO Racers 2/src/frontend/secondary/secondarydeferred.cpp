#include "frontend/secondary/secondarydeferred.h"

#include "core/debug/diagnostics.h"

#include <windows.h>

// Deliberate first-menu milestone boundaries. These entries do not implement
// retail behavior. Startup loads and cleanup live in the actual selector units;
// these branches require a later user choice, race/session action or disconnect.
static void FailDeferredSecondaryPath(const LegoChar* p_name)
{
	ReportFatalError("Unfinished post-menu operation: %s", p_name);
	ExitProcess(1);
}

// STUB: LEGORACERS2 0x00406930
void __fastcall RestartSecondaryBattle(void*, void*)
{
	FailDeferredSecondaryPath("RestartSecondaryBattle (original 00406930)");
}

// STUB: LEGORACERS2 0x0040e830
void __fastcall RestartSecondaryRace(void*, void*, LegoU32)
{
	FailDeferredSecondaryPath("RestartSecondaryRace (original 0040e830)");
}

// STUB: LEGORACERS2 0x0040e870
void __fastcall StartSecondarySessionRace(void*, void*, LegoU32, LegoU32, LegoU32)
{
	FailDeferredSecondaryPath("StartSecondarySessionRace (original 0040e870)");
}

// STUB: LEGORACERS2 0x00464240
LegoS32 __fastcall UpdateSecondaryCharacterSelection(void*, void*, LegoU32, SecondaryManagerMediaState*)
{
	FailDeferredSecondaryPath("UpdateSecondaryCharacterSelection (original 00464240)");
	return 0;
}

// STUB: LEGORACERS2 0x004644d0
void __fastcall BeginSecondaryCharacterSelection(void*, void*, LegoU32)
{
	FailDeferredSecondaryPath("BeginSecondaryCharacterSelection (original 004644d0)");
}

// STUB: LEGORACERS2 0x00467ff0
void __fastcall BeginSecondaryCarSelection(void*, void*, LegoU32, LegoS32, LegoU32)
{
	FailDeferredSecondaryPath("BeginSecondaryCarSelection (original 00467ff0)");
}

// STUB: LEGORACERS2 0x00468350
LegoS32 __fastcall UpdateSecondaryCarSelection(void*, void*, LegoU32, SecondaryManagerMediaState*)
{
	FailDeferredSecondaryPath("UpdateSecondaryCarSelection (original 00468350)");
	return 0;
}

// STUB: LEGORACERS2 0x00468e30
void __fastcall FinishSecondaryCarSelection(void*, void*)
{
	FailDeferredSecondaryPath("FinishSecondaryCarSelection (original 00468e30)");
}

// STUB: LEGORACERS2 0x00494de0
void __fastcall SendSecondaryNetworkRestart(void*, void*)
{
	FailDeferredSecondaryPath("SendSecondaryNetworkRestart (original 00494de0)");
}

// STUB: LEGORACERS2 0x00494e20
void __fastcall SendSecondaryNetworkNextRace(void*, void*)
{
	FailDeferredSecondaryPath("SendSecondaryNetworkNextRace (original 00494e20)");
}

// STUB: LEGORACERS2 0x00494e60
void __fastcall SendSecondaryNetworkContinue(void*, void*)
{
	FailDeferredSecondaryPath("SendSecondaryNetworkContinue (original 00494e60)");
}

// STUB: LEGORACERS2 0x00497280
void __fastcall RequestSecondaryNetworkExit(void*, void*)
{
	FailDeferredSecondaryPath("RequestSecondaryNetworkExit (original 00497280)");
}
