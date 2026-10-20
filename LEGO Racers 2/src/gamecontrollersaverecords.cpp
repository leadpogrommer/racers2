#include "gamecontrollerinitialization.h"

#include <string.h>

static void InitializeControllerSaveStateHalf(LegoU8* p_state)
{
	((GameControllerName*) (p_state + 8))[0].Initialize();
	((GameControllerName*) (p_state + 8))[1].Initialize();
	memset(p_state + 0x30, 0, 0x20);
	// The ten car-record constructor calls are no-write409190 bodies.
	*(LegoU32*) (p_state + 0x1df4) = 0;
	*(LegoU32*) (p_state + 0x1df8) = 0;
	*(LegoU32*) (p_state + 0x1dfc) = 0;
	((GameControllerSelection*) (p_state + 0x1e04))->Initialize();
	*(LegoU32*) (p_state + 0x1e24) = 0;
}

// STUB: LEGORACERS2 0x00410fc0
void* __fastcall ConstructControllerSaveRecords(void* p_storage, void*)
{
	LegoU8* state = (LegoU8*) p_storage;
	InitializeControllerSaveStateHalf(state);
	InitializeControllerSaveStateHalf(state + 0x1e34);
	*(LegoU32*) (state + 0x3c68) = 0;
	*(LegoU32*) (state + 0x3c6c) = 0;
	*(LegoU32*) (state + 0x3c70) = 0;
	*(LegoU32*) state = 0;
	*(LegoU32*) (state + 0x1e34) = 0;
	return p_storage;
}
