#ifndef LR2_INPUTMOUSE_H
#define LR2_INPUTMOUSE_H

#include "inputdevice.h"

struct MouseInputDevice : InputDeviceRecord {
	DIMOUSESTATE m_state; // 0xbc, 16-byte DirectInput mouse state

	LegoBool32 ReadState(LegoU32 p_unk0x4);
};

DECOMP_SIZE_ASSERT(DIMOUSESTATE, 0x10);
DECOMP_STATIC_ASSERT(offsetof(MouseInputDevice, m_state) == 0xbc);

#endif // LR2_INPUTMOUSE_H
