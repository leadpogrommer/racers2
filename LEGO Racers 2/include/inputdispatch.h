#ifndef LR2_INPUTDISPATCH_H
#define LR2_INPUTDISPATCH_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

struct InputDeviceRecord;

// Observed game dispatch layout, separate from the SDK's DirectInput COM ABI.
// The bridges use ECX for the receiver and leave EDX unused.
struct InputDeviceDispatch {
	typedef LegoBool32(__fastcall* Predicate)(InputDeviceRecord*, void*);
	typedef LegoBool32(__fastcall* IndexedPredicate)(InputDeviceRecord*, void*, LegoU32);
	typedef LegoFloat(__fastcall* IndexedFloat)(InputDeviceRecord*, void*, LegoU32);
	typedef void(__fastcall* Action)(InputDeviceRecord*, void*);
	typedef void(__fastcall* OneArgument)(InputDeviceRecord*, void*, LegoU32);
	typedef void(__fastcall* TwoArguments)(InputDeviceRecord*, void*, LegoU32, LegoU32);
	typedef void(__fastcall* ThreeArguments)(InputDeviceRecord*, void*, LegoU32, LegoU32, LegoU32);

	IndexedPredicate readState;       // 0x00
	IndexedPredicate isButtonPressed; // 0x04
	IndexedFloat getButtonValue;      // 0x08
	IndexedFloat getAxisValue;        // 0x0c
	Predicate VTable0x10;             // 0x10
	Predicate VTable0x14;             // 0x14
	OneArgument performAction;        // 0x18
	Predicate VTable0x1c;             // 0x1c
	Action cleanup;                   // 0x20
	Predicate isActive;               // 0x24
	TwoArguments VTable0x28;          // 0x28
	OneArgument VTable0x2c;           // 0x2c
	OneArgument VTable0x30;           // 0x30
	OneArgument VTable0x34;           // 0x34
	TwoArguments VTable0x38;          // 0x38
	ThreeArguments VTable0x3c;        // 0x3c
	ThreeArguments VTable0x40;        // 0x40
	Action VTable0x44;                // 0x44
	Action VTable0x48;                // 0x48
	Action VTable0x4c;                // 0x4c
	Action VTable0x50;                // 0x50
	Action VTable0x54;                // 0x54
	Action VTable0x58;                // 0x58
	Action VTable0x5c;                // 0x5c
	Action VTable0x60;                // 0x60
};

DECOMP_SIZE_ASSERT(InputDeviceDispatch, 0x64);
DECOMP_STATIC_ASSERT(offsetof(InputDeviceDispatch, performAction) == 0x18);
DECOMP_STATIC_ASSERT(offsetof(InputDeviceDispatch, cleanup) == 0x20);
DECOMP_STATIC_ASSERT(offsetof(InputDeviceDispatch, isActive) == 0x24);
DECOMP_STATIC_ASSERT(offsetof(InputDeviceDispatch, VTable0x60) == 0x60);

extern const InputDeviceDispatch g_mouseInputDispatch;
extern const InputDeviceDispatch g_keyboardInputDispatch;

#endif // LR2_INPUTDISPATCH_H
