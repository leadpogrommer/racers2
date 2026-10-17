#include "platformfpu.h"

#include "types.h"

// GLOBAL: LEGORACERS2 0x006843ac
static LegoU16 g_savedFloatingPointControl = 0;

// x87 control registers and FINIT have no equivalent portable C++ operation.
// Use the actual hardware instructions here, not byte-encoded function bodies.
// STUB: LEGORACERS2 0x004e7b10
void InitializeRendererFloatingPoint()
{
	LegoU16 controlWord;
	// clang-format off
	__asm {
		finit
		fstcw controlWord
	}
	// clang-format on
	controlWord &= 0xfcff;
	// clang-format off
	__asm {
		fldcw controlWord
		fwait
	}
	// clang-format on
}

// FUNCTION: LEGORACERS2 0x004e7b30
void SaveFloatingPointControl()
{
	__asm { fstcw g_savedFloatingPointControl }
}

// FUNCTION: LEGORACERS2 0x004e7b40
void RestoreFloatingPointControl()
{
	__asm { fldcw g_savedFloatingPointControl }
}
