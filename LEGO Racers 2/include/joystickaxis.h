#ifndef LR2_JOYSTICKAXIS_H
#define LR2_JOYSTICKAXIS_H

#include "decomp.h"
#include "directx8.h"
#include "types.h"

#include <stddef.h>

class JoystickAxis {
public:
	JoystickAxis();
	void Bind(LegoS32* p_stateBuffer, LegoU32 p_byteOffset, LPDIRECTINPUTDEVICE8A p_device);
	LegoU32 SetRange(LegoFloat p_min, LegoFloat p_max);
	LegoFloat GetNormalizedValue();
	void ExpandRange();

	LPDIRECTINPUTDEVICE8A m_device; // 0x00
	LegoU32 m_objectOffset;         // 0x04
	LegoU32 m_softwareRange;        // 0x08
	LegoS32* m_state;               // 0x0c
	LegoFloat m_range;              // 0x10
	LegoFloat m_min;                // 0x14
	LegoFloat m_max;                // 0x18
};

DECOMP_SIZE_ASSERT(JoystickAxis, 0x1c);
DECOMP_STATIC_ASSERT(offsetof(JoystickAxis, m_device) == 0x00);
DECOMP_STATIC_ASSERT(offsetof(JoystickAxis, m_objectOffset) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(JoystickAxis, m_softwareRange) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(JoystickAxis, m_state) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(JoystickAxis, m_range) == 0x10);
DECOMP_STATIC_ASSERT(offsetof(JoystickAxis, m_min) == 0x14);
DECOMP_STATIC_ASSERT(offsetof(JoystickAxis, m_max) == 0x18);

extern HRESULT g_lastJoystickInputResult;

#endif // LR2_JOYSTICKAXIS_H
