#include "controllerdeferred.h"

#include "diagnostics.h"
#include "gamecontrollerinitialization.h"

#include <stdlib.h>

// Deliberately unfinished post-menu boundary. GameController::Initialize leaves
// m_state=0. Its first-menu Update case0 calls only SecondaryManager::BeginStartup.
// The functions below are used after a menu action requests a scene/race, or by
// the battle state machine after its constructor's idle state8 is changed.
// They must never be reported as recovered behavior or promoted to FUNCTION.
static __declspec(noreturn) void UnfinishedControllerFunction(LegoU32 p_originalAddress)
{
	ReportFatalError("Unfinished post-menu controller function at original VA 0x%08x", p_originalAddress);
	abort();
}
void __fastcall RunDeferredBattleState(GameObject*, void*, LegoU32)
{
	UnfinishedControllerFunction(0x004060b0);
}

// STUB: LEGORACERS2 0x00406960
void __fastcall FinishSecondaryBattle(void*, void*)
{
	UnfinishedControllerFunction(0x00406960);
}
// STUB: LEGORACERS2 0x00408910
void* __fastcall BuildControllerModeTemplate(GameController*, void*, LegoS32)
{
	UnfinishedControllerFunction(0x00408910);
}
// STUB: LEGORACERS2 0x00408bf0
void __fastcall ConfigureControllerSelectionLayout(GameController*, void*, LegoFloat, LegoFloat)
{
	UnfinishedControllerFunction(0x00408bf0);
}
// STUB: LEGORACERS2 0x004091e0
void __fastcall RefreshControllerSelection(GameController*, void*)
{
	UnfinishedControllerFunction(0x004091e0);
}
// STUB: LEGORACERS2 0x004092d0
void __fastcall BuildControllerSelectionWidgets(GameController*, void*)
{
	UnfinishedControllerFunction(0x004092d0);
}
// STUB: LEGORACERS2 0x00409440
void __fastcall UpdateControllerScene(GameController*, void*)
{
	UnfinishedControllerFunction(0x00409440);
}
// STUB: LEGORACERS2 0x00409550
void __fastcall ResetControllerSelectionWidgets(GameController*, void*)
{
	UnfinishedControllerFunction(0x00409550);
}
// STUB: LEGORACERS2 0x0040b9c0
GameObject* __fastcall ConstructRaceManager(void*, void*, GameObjectTemplate*)
{
	UnfinishedControllerFunction(0x0040b9c0);
}
// STUB: LEGORACERS2 0x0040e8e0
void __fastcall FinishSecondaryRace(void*, void*, LegoU32)
{
	UnfinishedControllerFunction(0x0040e8e0);
}
// STUB: LEGORACERS2 0x0040eb40
void __fastcall FUN_0040eb40(GameObject*, void*)
{
	UnfinishedControllerFunction(0x0040eb40);
}
// STUB: LEGORACERS2 0x00411110
void __fastcall FUN_00411110(void*, void*)
{
	UnfinishedControllerFunction(0x00411110);
}
// STUB: LEGORACERS2 0x00411670
void __fastcall StartNewMenuSession(void*, void*)
{
	UnfinishedControllerFunction(0x00411670);
}
// STUB: LEGORACERS2 0x00432e40
void* __fastcall ConstructControllerHudManager(void*, void*, GameObjectTemplate*)
{
	UnfinishedControllerFunction(0x00432e40);
}
// STUB: LEGORACERS2 0x004487b0
LegoU32 __cdecl SelectControllerSceneRecord(LegoU32)
{
	UnfinishedControllerFunction(0x004487b0);
}
// STUB: LEGORACERS2 0x0044abd0
void __fastcall SetControllerMenuMode(void*, void*, LegoU32)
{
	UnfinishedControllerFunction(0x0044abd0);
}
// STUB: LEGORACERS2 0x0049ae10
void* __fastcall ConstructControllerModeMenu(void*, void*, void*)
{
	UnfinishedControllerFunction(0x0049ae10);
}
// STUB: LEGORACERS2 0x0049b190
void __fastcall AddControllerSceneLabel(void*, void*, const CharBuffer24*, LegoU32)
{
	UnfinishedControllerFunction(0x0049b190);
}
// STUB: LEGORACERS2 0x0049b340
void __fastcall SetControllerSceneProgressRange(void*, void*, LegoFloat, LegoFloat)
{
	UnfinishedControllerFunction(0x0049b340);
}
