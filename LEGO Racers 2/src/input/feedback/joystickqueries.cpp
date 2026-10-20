#include "input/devices/inputjoystick.h"
#include "input/service/inputsetup.h"

#include <string.h>

// FUNCTION: LEGORACERS2 0x004ed940
LegoBool32 JoystickInputDevice::HasForceFeedbackAxes()
{
	return m_forceFeedbackAxisCount > 0;
}

// STUB: LEGORACERS2 0x004edd50
LegoBool32 JoystickInputDevice::ReadState(LegoU32 p_mode)
{
	(void) p_mode;
	if (m_device != NULL) {
		memset(&m_capabilities, 0, sizeof(m_capabilities));
		m_capabilities.dwSize = sizeof(m_capabilities);
		g_lastJoystickInputResult = m_device->GetCapabilities(&m_capabilities);

		if (g_lastJoystickInputResult != DI_OK || (m_capabilities.dwFlags & 1) == 0) {
			memset(&m_state, 0, sizeof(m_state));
		}
		else {
			if ((m_capabilities.dwFlags & 2) != 0 && (m_capabilities.dwFlags & 8) != 0) {
				g_lastJoystickInputResult = m_device->Poll();
				if (g_lastJoystickInputResult != DI_OK) {
					g_lastJoystickInputResult = m_device->Acquire();
					if (g_lastJoystickInputResult != DI_OK) {
						goto update_auto_range;
					}
					g_lastJoystickInputResult = m_device->Poll();
					if (g_lastJoystickInputResult != DI_OK) {
						goto update_auto_range;
					}
				}
			}

			g_lastJoystickInputResult = m_device->GetDeviceState(sizeof(m_state), &m_state);
			if (g_lastJoystickInputResult != DI_OK) {
				g_lastJoystickInputResult = m_device->Acquire();
				if (g_lastJoystickInputResult != DI_OK) {
					goto update_auto_range;
				}
				g_lastJoystickInputResult = m_device->GetDeviceState(sizeof(m_state), &m_state);
			}
		}
	}

update_auto_range:
	UpdateAutoRange();
	return TRUE;
}

// FUNCTION: LEGORACERS2 0x004edf10
LegoS32 JoystickInputDevice::GetAxisCount()
{
	return m_axisCount;
}

// STUB: LEGORACERS2 0x004edf20
void JoystickInputDevice::UpdateAutoRange()
{
	if (!m_autoRangeInitialized && m_device != NULL &&
		(m_state.lX != 0 || m_state.lY != 0 || m_state.lZ != 0 || m_state.lRx != 0 || m_state.lRy != 0 ||
		 m_state.lRz != 0 || m_state.rglSlider[0] != 0 || m_state.rglSlider[1] != 0)) {
		m_autoRangeInitialized = TRUE;
		if (m_capabilities.dwAxes > 2 && m_axisCount > 1) {
			for (LegoS32 i = 1; i < m_axisCount; i++) {
				m_axes[i].ExpandRange();
			}
		}
	}
}

// FUNCTION: LEGORACERS2 0x004ee020
LegoBool32 JoystickInputDevice::IsButtonPressed(LegoU32 p_buttonIndex)
{
	if (p_buttonIndex >= m_capabilities.dwButtons) {
		return FALSE;
	}
	return m_state.rgbButtons[p_buttonIndex] != 0;
}

// FUNCTION: LEGORACERS2 0x004ee080
LegoFloat JoystickInputDevice::GetAxisValue(LegoS32 p_axisIndex)
{
	if (p_axisIndex >= m_axisCount || p_axisIndex < 0) {
		return 0.0f;
	}
	return m_axes[p_axisIndex].GetNormalizedValue();
}

// FUNCTION: LEGORACERS2 0x004ee0c0
LegoBool32 JoystickInputDevice::Acquire()
{
	g_lastJoystickInputResult = m_device->Acquire();
	m_isAcquired = TRUE;
	CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputDevice8_Acquire");
	return g_lastJoystickInputResult == DI_OK;
}

// FUNCTION: LEGORACERS2 0x004ee110
LegoBool32 JoystickInputDevice::Unacquire()
{
	m_isAcquired = FALSE;
	g_lastJoystickInputResult = m_device->Unacquire();
	CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputDevice8_Unacquire");
	return g_lastJoystickInputResult == DI_OK;
}
