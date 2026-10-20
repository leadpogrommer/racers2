#include "input/devices/inputdevice.h"

#include "input/devices/inputkeyboard.h"
#include "input/devices/inputmouse.h"
#include "input/service/inputdeviceservice.h"

#include <string.h>

// GLOBAL: LEGORACERS2 0x00653114
HRESULT g_lastDirectInputResult = 0;

// GLOBAL: LEGORACERS2 0x006e4554
HRESULT g_lastMouseInputResult = 0;

// GLOBAL: LEGORACERS2 0x006e455c
HRESULT g_lastKeyboardInputResult = 0;

// FUNCTION: LEGORACERS2 0x004af130
void InputDeviceService::AcquireAvailableDevices()
{
	for (LegoU32 i = 0; i < m_deviceCount; i++) {
		InputDeviceRecord* device = m_devices[i];
		if (device->m_isAvailable) {
			g_lastDirectInputResult = device->m_device->Acquire();
		}
	}
}

// STUB: LEGORACERS2 0x004af1a0
InputDeviceRecord* InputDeviceService::FindDeviceByType(LegoU32 p_index, LegoU32 p_type)
{
	LegoU32 count = m_deviceCount;
	LegoU32 typeIndex = 0;
	InputDeviceRecord** device = m_devices;
	for (LegoU32 i = 0; i < count; i++) {
		if ((*device)->m_deviceType == p_type) {
			if (typeIndex == p_index) {
				return *device;
			}
			typeIndex++;
		}
		device++;
	}
	return NULL;
}

// FUNCTION: LEGORACERS2 0x004ec470
LegoBool32 MouseInputDevice::ReadState(LegoU32 p_unk0x4)
{
	(void) p_unk0x4;
	if (m_device != NULL) {
		g_lastMouseInputResult = m_device->GetDeviceState(sizeof(m_state), &m_state);
		if (g_lastMouseInputResult != 0) {
			g_lastMouseInputResult = m_device->Acquire();
			if (g_lastMouseInputResult == 0) {
				g_lastMouseInputResult = m_device->GetDeviceState(sizeof(m_state), &m_state);
			}
		}
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004ec590
LegoBool32 KeyboardInputDevice::ReadState(LegoU32 p_unk0x4)
{
	(void) p_unk0x4;
	if (m_device != NULL) {
		g_lastKeyboardInputResult = m_device->GetDeviceState(sizeof(m_keyState), m_keyState);
		if (g_lastKeyboardInputResult != 0) {
			g_lastKeyboardInputResult = m_device->Acquire();
			if (g_lastKeyboardInputResult == 0) {
				g_lastKeyboardInputResult = m_device->GetDeviceState(sizeof(m_keyState), m_keyState);
			}
			else {
				memset(m_keyState, 0, sizeof(m_keyState));
			}
		}
	}
	return TRUE;
}
