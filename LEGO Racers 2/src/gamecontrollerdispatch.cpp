#include "gamecontroller.h"
#include "gameobjectdispatch.h"
#include "memoryheap.h"

static GameObject* __fastcall DeleteGameController(GameObject*, void*, LegoU32);
static LegoU32 __fastcall GameControllerAllocationSize(GameObject*, void*);
static void __fastcall InitializeGameController(GameObject* p_object, void*)
{
	((GameController*) p_object)->Initialize();
}
static LegoU32 __fastcall UpdateGameController(GameObject* p_object, void*, LegoU32 p_elapsed)
{
	return ((GameController*) p_object)->Update(p_elapsed);
}
static void __fastcall RemoveGameController(GameObject* p_object, void*)
{
	((GameController*) p_object)->Remove();
}

// Retail 0x0052f528: all twenty base callbacks plus the size slot. The next
// pointer at 0x0052f57c starts a different object's table.
extern const GameControllerDispatch g_gameControllerDispatch = {
	{DeleteGameController,       GameObjectBaseApplyChildTransform,
	 GameObjectBaseNoOp,         GameObjectBaseNoOp,
	 InitializeGameController,   GameObjectBaseTrue,
	 UpdateGameController,       RemoveGameController,
	 GameObjectBaseZero,         GameObjectBaseZero,
	 GameObjectBaseNoOp,         GameObjectBaseNoOp,
	 GameObjectBaseDrawChild,    GameObjectBaseNoOpOneArgument,
	 GameObjectBaseZero,         GameObjectBaseNoOpTwoArguments,
	 GameObjectBaseZero,         GameObjectBaseZeroTwoArguments,
	 GameObjectBaseUpdateRegion, GameObjectBaseNoOpOneArgument},
	GameControllerAllocationSize
};

// STUB: LEGORACERS2 0x004071c0
static LegoU32 __fastcall GameControllerAllocationSize(GameObject*, void*)
{
	return 0x9be0;
}

// STUB: LEGORACERS2 0x004071d0
static GameObject* __fastcall DeleteGameController(GameObject* p_object, void*, LegoU32 p_flags)
{
	((GameController*) p_object)->~GameController();
	if (p_flags & 1) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}
