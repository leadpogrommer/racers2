#ifndef LR2_GAMEOBJECTDISPATCH_H
#define LR2_GAMEOBJECTDISPATCH_H

#include "gamemanager.h"

// Typed bridges for the retail GameObject base dispatch targets. Derived
// static tables can use the initializer below while keeping their own table
// address in the object image.
GameObject* __fastcall GameObjectBaseDestroy(GameObject*, void*, LegoU32);
void __fastcall GameObjectBaseApplyChildTransform(GameObject*, void*, LegoU32, LegoU32);
void __fastcall GameObjectBaseNoOp(GameObject*, void*);
LegoU32 __fastcall GameObjectBaseTrue(GameObject*, void*);
LegoU32 __fastcall GameObjectBaseZero(GameObject*, void*);
LegoU32 __fastcall GameObjectBaseZeroOneArgument(GameObject*, void*, LegoU32);
void __fastcall GameObjectBaseNoOpOneArgument(GameObject*, void*, LegoU32);
void __fastcall GameObjectBaseNoOpTwoArguments(GameObject*, void*, LegoU32, LegoU32);
LegoU32 __fastcall GameObjectBaseZeroTwoArguments(GameObject*, void*, LegoU32, LegoU32);
void __fastcall GameObjectBaseDrawChild(GameObject*, void*);
void __fastcall GameObjectBaseUpdateRegion(GameObject*, void*, LegoU32);
void __fastcall GameObjectBaseInitialize(GameObject*, void*);
void __fastcall GameObjectBaseRemove(GameObject*, void*);

#define GAME_OBJECT_BASE_DISPATCH_INITIALIZER                                                                          \
	{&GameObjectBaseDestroy,                                                                                           \
	 &GameObjectBaseApplyChildTransform,                                                                               \
	 &GameObjectBaseNoOp,                                                                                              \
	 &GameObjectBaseNoOp,                                                                                              \
	 &GameObjectBaseInitialize,                                                                                        \
	 &GameObjectBaseTrue,                                                                                              \
	 &GameObjectBaseZeroOneArgument,                                                                                   \
	 &GameObjectBaseRemove,                                                                                            \
	 &GameObjectBaseZero,                                                                                              \
	 &GameObjectBaseZero,                                                                                              \
	 &GameObjectBaseNoOp,                                                                                              \
	 &GameObjectBaseNoOp,                                                                                              \
	 &GameObjectBaseDrawChild,                                                                                         \
	 &GameObjectBaseNoOpOneArgument,                                                                                   \
	 &GameObjectBaseZero,                                                                                              \
	 &GameObjectBaseNoOpTwoArguments,                                                                                  \
	 &GameObjectBaseZero,                                                                                              \
	 &GameObjectBaseZeroTwoArguments,                                                                                  \
	 &GameObjectBaseUpdateRegion,                                                                                      \
	 &GameObjectBaseNoOpOneArgument}

#endif // LR2_GAMEOBJECTDISPATCH_H
