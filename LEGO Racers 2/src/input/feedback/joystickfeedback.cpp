#include "input/devices/inputjoystick.h"
#include "input/service/inputsetup.h"

// FUNCTION: LEGORACERS2 0x004edec0
void JoystickInputDevice::StopAllEffects()
{
	for (LegoU32 i = 0; i < 7; i++) {
		m_effects[i].Stop();
	}
}

// STUB: LEGORACERS2 0x004edee0
void JoystickInputDevice::EnableForceFeedback(LegoU32 p_enabled)
{
	if (p_enabled == 0) {
		m_forceFeedbackEnabled = FALSE;
		m_vtable->VTable0x44(this, NULL);
	}
	else {
		m_forceFeedbackEnabled = TRUE;
	}
}

// FUNCTION: LEGORACERS2 0x004ee160
void JoystickInputDevice::ResetForceFeedback()
{
	g_lastJoystickInputResult = m_device->SendForceFeedbackCommand(DISFFC_RESET);
	CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputDevice8_SendForceFeedbackCommand");
}

// STUB: LEGORACERS2 0x004ee190
void JoystickInputDevice::InitializeEffects(JoystickInputDevice*)
{
	ResetForceFeedback();
	// Each temporary has its own constructor call and the retail copy includes
	// all 0xe4 bytes. The seven kinds are distinct even where GUIDs are shared.
	m_effects[0] = JoystickEffect(m_device, 0, m_forceFeedbackAxisCount);
	CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputDevice8_CreateEffect");
	m_effects[3] = JoystickEffect(m_device, 3, m_forceFeedbackAxisCount);
	CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputDevice8_CreateEffect");
	m_effects[1] = JoystickEffect(m_device, 1, m_forceFeedbackAxisCount);
	CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputDevice8_CreateEffect");
	m_effects[2] = JoystickEffect(m_device, 2, m_forceFeedbackAxisCount);
	CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputDevice8_CreateEffect");
	m_effects[4] = JoystickEffect(m_device, 4, m_forceFeedbackAxisCount);
	CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputDevice8_CreateEffect");
	m_effects[5] = JoystickEffect(m_device, 5, m_forceFeedbackAxisCount);
	CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputDevice8_CreateEffect");
	m_effects[6] = JoystickEffect(m_device, 6, m_forceFeedbackAxisCount);
	CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputDevice8_CreateEffect");
	Unacquire();
	SetAutoCenter(FALSE);
}

// STUB: LEGORACERS2 0x004ee3d0
void JoystickInputDevice::SetConstantForce(LegoU32 p_axis, LegoFloat p_force)
{
	if (m_forceFeedbackEnabled) {
		if (p_axis == 0 && m_axisCount > 0) {
			m_effects[0].SetConstantForce(p_force, 0.0f);
		}
		else if (p_axis == 1 && m_axisCount > 1) {
			m_effects[0].SetConstantForce(0.0f, p_force);
		}
	}
}

// FUNCTION: LEGORACERS2 0x004ee430
void JoystickInputDevice::SetSpring(LegoFloat p_value)
{
	if (m_forceFeedbackEnabled) {
		m_effects[2].SetSpringCoefficient(p_value);
	}
}

// FUNCTION: LEGORACERS2 0x004ee450
void JoystickInputDevice::SetFriction(LegoFloat p_value)
{
	if (m_forceFeedbackEnabled) {
		m_effects[1].SetFrictionCoefficient(p_value);
	}
}

// FUNCTION: LEGORACERS2 0x004ee470
void JoystickInputDevice::SetSine(LegoFloat p_first, LegoFloat p_second)
{
	if (m_forceFeedbackEnabled) {
		m_effects[3].SetPeriodicParameters(p_first, p_second);
	}
}

// FUNCTION: LEGORACERS2 0x004ee4a0
void JoystickInputDevice::SetEnvelopedConstantForce(LegoFloat p_value)
{
	if (m_forceFeedbackEnabled) {
		m_effects[5].SetEnvelopeConstantForce(p_value, p_value);
	}
}

// FUNCTION: LEGORACERS2 0x004ee4c0
void JoystickInputDevice::SetSquare(LegoFloat p_first, LegoFloat p_second, LegoFloat p_third)
{
	if (m_forceFeedbackEnabled) {
		m_effects[4].SetSquareParameters(p_first, p_second, p_third);
	}
}

// FUNCTION: LEGORACERS2 0x004ee4f0
void JoystickInputDevice::SetEnvelopedSine(LegoFloat p_first, LegoFloat p_second, LegoFloat p_third)
{
	if (m_forceFeedbackEnabled) {
		m_effects[6].SetEnvelopePeriodicParameters(p_first, p_second, p_third);
	}
}

// FUNCTION: LEGORACERS2 0x004ee520
void JoystickInputDevice::StopConstantForce()
{
	if (m_forceFeedbackEnabled) {
		m_effects[0].Stop();
	}
}

// FUNCTION: LEGORACERS2 0x004ee540
void JoystickInputDevice::StopEnvelopedSine()
{
	if (m_forceFeedbackEnabled) {
		m_effects[6].Stop();
	}
}

// FUNCTION: LEGORACERS2 0x004ee560
void JoystickInputDevice::StopSpring()
{
	if (m_forceFeedbackEnabled) {
		m_effects[2].Stop();
	}
}

// FUNCTION: LEGORACERS2 0x004ee580
void JoystickInputDevice::StopFriction()
{
	if (m_forceFeedbackEnabled) {
		m_effects[1].Stop();
	}
}

// FUNCTION: LEGORACERS2 0x004ee5a0
void JoystickInputDevice::StopSine()
{
	if (m_forceFeedbackEnabled) {
		m_effects[3].Stop();
	}
}

// FUNCTION: LEGORACERS2 0x004ee5c0
void JoystickInputDevice::StopSquare()
{
	if (m_forceFeedbackEnabled) {
		m_effects[4].Stop();
	}
}

// FUNCTION: LEGORACERS2 0x004ee5e0
void JoystickInputDevice::StopEnvelopedConstantForce()
{
	if (m_forceFeedbackEnabled) {
		m_effects[5].Stop();
	}
}

// FUNCTION: LEGORACERS2 0x004ee600
void JoystickInputDevice::ReleaseEffects()
{
	g_lastJoystickInputResult = m_device->SendForceFeedbackCommand(DISFFC_STOPALL);
	CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputDevice8_SendForceFeedbackCommand");
	for (LegoU32 i = 0; i < 7; i++) {
		m_effects[i].Release();
	}
	ResetForceFeedback();
}

// FUNCTION: LEGORACERS2 0x004ee660
void JoystickInputDevice::SetAutoCenter(LegoBool32 p_enabled)
{
	DIPROPDWORD property;
	property.diph.dwSize = sizeof(property);
	property.diph.dwHeaderSize = sizeof(property.diph);
	property.diph.dwObj = 0;
	property.diph.dwHow = DIPH_DEVICE;
	property.dwData = p_enabled != FALSE;
	g_lastJoystickInputResult = m_device->SetProperty(DIPROP_AUTOCENTER, &property.diph);
}
