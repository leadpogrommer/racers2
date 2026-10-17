#include "joystickaxis.h"

// GLOBAL: LEGORACERS2 0x006e4634
HRESULT g_lastJoystickInputResult = 0;

// FUNCTION: LEGORACERS2 0x004edbf0
JoystickAxis::JoystickAxis()
{
	m_objectOffset = 0;
	m_softwareRange = 0;
	m_state = NULL;
	m_device = NULL;
	m_range = 1.0f;
	m_min = 0.0f;
	m_max = 0.0f;
}

// FUNCTION: LEGORACERS2 0x004edc10
void JoystickAxis::Bind(LegoS32* p_stateBuffer, LegoU32 p_byteOffset, LPDIRECTINPUTDEVICE8A p_device)
{
	m_device = p_device;
	m_objectOffset = p_byteOffset;
	m_state = (LegoS32*) ((LegoU8*) p_stateBuffer + p_byteOffset);
	m_softwareRange = SetRange(-255.0f, 255.0f);
}

// STUB: LEGORACERS2 0x004edc50
LegoU32 JoystickAxis::SetRange(LegoFloat p_min, LegoFloat p_max)
{
	DIPROPRANGE property;
	LegoS32 integerMin;
	LegoS32 integerMax;

	property.diph.dwSize = sizeof(property);
	property.diph.dwHeaderSize = sizeof(property.diph);
	property.diph.dwObj = m_objectOffset;
	property.diph.dwHow = DIPH_BYOFFSET;

	// Retail uses FISTP, so preserve the active x87 rounding mode.
	__asm {
		fld p_min
		fistp integerMin
		fld p_max
		fistp integerMax
	}

	property.lMin = integerMin;
	property.lMax = integerMax;
	g_lastJoystickInputResult = m_device->SetProperty(DIPROP_RANGE, &property.diph);

	if (g_lastJoystickInputResult == DI_OK) {
		m_max = p_max;
		m_min = p_min;
		m_softwareRange = FALSE;
		m_range = m_max - m_min;
		return m_softwareRange;
	}

	m_softwareRange = TRUE;
	m_min = (LegoFloat) integerMin;
	m_max = (LegoFloat) integerMax;
	m_range = m_max - m_min;
	return m_softwareRange;
}

// FUNCTION: LEGORACERS2 0x004edcf0
LegoFloat JoystickAxis::GetNormalizedValue()
{
	if (m_softwareRange == FALSE) {
		return 0.0f;
	}

	LegoFloat value = ((LegoFloat) *m_state - m_min) / m_range;
	return value + value - 1.0f;
}

// FUNCTION: LEGORACERS2 0x004edfc0
void JoystickAxis::ExpandRange()
{
	if (m_softwareRange != FALSE) {
		LegoFloat value = ((LegoFloat) *m_state - m_min) / m_range;
		value = value + value - 1.0f;
		if (value > 0.5f) {
			m_max = m_max + m_range;
			m_range = m_max - m_min;
			return;
		}

		if (!(value >= -0.5f)) {
			m_min = m_min - m_range;
			m_range = m_max - m_min;
		}
	}
}
