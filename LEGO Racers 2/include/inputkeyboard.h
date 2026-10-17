#ifndef LR2_INPUTKEYBOARD_H
#define LR2_INPUTKEYBOARD_H

#include "inputdevice.h"

struct KeyboardInputDevice : InputDeviceRecord {
	BYTE m_keyState[256]; // 0xbc, DirectInput keyboard state

	KeyboardInputDevice();
	LegoBool32 ReadState(LegoU32 p_unk0x4);
	LegoBool32 IsKeyPressed(LegoU32 p_keyCode);
};

DECOMP_STATIC_ASSERT(offsetof(KeyboardInputDevice, m_keyState) == 0xbc);

#endif // LR2_INPUTKEYBOARD_H
