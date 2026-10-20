#include "input/devices/inputdevice.h"
#include "input/devices/inputkeyboard.h"
#include "input/devices/inputmouse.h"

// GLOBAL: LEGORACERS2 0x00536e5c
LegoFloat g_mouseAxisScale = 0.01568627543747425f;

// FUNCTION: LEGORACERS2 0x004ec4d0
LegoBool32 MouseInputDevice::IsButtonPressed(LegoU32 p_buttonIndex)
{
	if (p_buttonIndex >= m_capabilities.dwButtons) {
		return FALSE;
	}
	return m_state.rgbButtons[p_buttonIndex] != 0;
}

// STUB: LEGORACERS2 0x004ec500
LegoFloat MouseInputDevice::GetAxisValue(LegoU32 p_axis)
{
	LegoS32 rawValue;
	switch (p_axis) {
	case 0:
		rawValue = m_state.lX;
		break;
	case 1:
		rawValue = m_state.lY;
		break;
	case 2:
		rawValue = m_state.lZ;
		break;
	default:
		return 0.0f;
	}

	LegoFloat value = rawValue * g_mouseAxisScale;
	if (!(value >= -1.0f)) {
		return -1.0f;
	}
	if (value > 1.0f) {
		return 1.0f;
	}
	return value;
}

// FUNCTION: LEGORACERS2 0x004ec630
LegoBool32 KeyboardInputDevice::IsKeyPressed(LegoU32 p_keyCode)
{
	return m_keyState[p_keyCode] >> 7;
}

// FUNCTION: LEGORACERS2 0x004ee050
LegoFloat InputDeviceRecord::GetButtonValue(LegoU32 p_buttonIndex)
{
	if (m_vtable->isButtonPressed(this, NULL, p_buttonIndex) == TRUE) {
		return 1.0f;
	}
	return 0.0f;
}
