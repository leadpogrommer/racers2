#include "networkdeferred.h"

#include "diagnostics.h"

#include <windows.h>

// Deliberate first-menu milestone scaffolding. None of these bodies recovers
// the original behavior. Keep this unit separate so actual implementations can
// replace each entry without altering the implemented startup path.
static void FailDeferredNetworkPath(const LegoChar* p_name)
{
	ReportFatalError("Unfinished post-menu network operation: %s", p_name);
	ExitProcess(1);
}

// STUB: LEGORACERS2 0x00449640
void __fastcall SetNetworkRacerState(void*, void*, LegoU32)
{
	FailDeferredNetworkPath("SetNetworkRacerState (original 00449640)");
}

// STUB: LEGORACERS2 0x004940c0
void __fastcall SendNetworkPlayerCar(NetworkAppLayer*, void*, LegoS32)
{
	FailDeferredNetworkPath("SendNetworkPlayerCar (original 004940c0)");
}

// STUB: LEGORACERS2 0x00494170
void __fastcall SendNetworkPlayerCharacter(NetworkAppLayer*, void*, LegoS32)
{
	FailDeferredNetworkPath("SendNetworkPlayerCharacter (original 00494170)");
}

// STUB: LEGORACERS2 0x00494220
void __fastcall SendNetworkPlayerState(NetworkAppLayer*, void*, LegoS32)
{
	FailDeferredNetworkPath("SendNetworkPlayerState (original 00494220)");
}

// STUB: LEGORACERS2 0x00494420
void __fastcall BeginNetworkGame(NetworkAppLayer*, void*)
{
	FailDeferredNetworkPath("BeginNetworkGame (original 00494420)");
}

// STUB: LEGORACERS2 0x00494670
void __fastcall ProcessNetworkMessages(NetworkAppLayer*, void*, LegoFloat)
{
	FailDeferredNetworkPath("ProcessNetworkMessages (original 00494670)");
}

// STUB: LEGORACERS2 0x004949a0
LegoBool32 __fastcall PrepareNetworkGame(NetworkAppLayer*, void*, LegoFloat)
{
	FailDeferredNetworkPath("PrepareNetworkGame (original 004949a0)");
	return FALSE;
}

// STUB: LEGORACERS2 0x00494a50
void __fastcall ConfigureNetworkGame(NetworkAppLayer*, void*)
{
	FailDeferredNetworkPath("ConfigureNetworkGame (original 00494a50)");
}

// STUB: LEGORACERS2 0x00494ae0
void __fastcall FinishNetworkGame(NetworkAppLayer*, void*)
{
	FailDeferredNetworkPath("FinishNetworkGame (original 00494ae0)");
}

// STUB: LEGORACERS2 0x00494b20
void __fastcall InitializeNetworkLocalPlayer(NetworkAppLayer*, void*)
{
	FailDeferredNetworkPath("InitializeNetworkLocalPlayer (original 00494b20)");
}
