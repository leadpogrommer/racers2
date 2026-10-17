#include "joystickeffect.h"

#include "inputsetup.h"
#include "joystickaxis.h"
#include "joystickparams.h"
#include "joystickperiodic.h"

JoystickEffect::~JoystickEffect()
{
}

// FUNCTION: LEGORACERS2 0x004ec670
JoystickEffect::JoystickEffect()
{
	m_axisMode = 0;
	m_effect = NULL;
	m_isPlaying = 0;
	m_device = NULL;
}

// STUB: LEGORACERS2 0x004ec680
JoystickEffect::JoystickEffect(LPDIRECTINPUTDEVICE8A p_device, LegoU32 p_effectKind, LegoU32 p_axisMode)
{
	m_axisMode = p_axisMode > 2 ? 2 : p_axisMode;
	m_unk0xe0 = 0;
	m_effect = NULL;
	m_isPlaying = 0;
	m_device = p_device;
	if (p_device == NULL) {
		return;
	}

	DIEFFECT* description;
	switch (p_effectKind) {
	case 0:
		description = BuildConstantForceDescription(m_axisMode, 0.0f, 0.0f);
		g_lastJoystickInputResult = p_device->CreateEffect(GUID_ConstantForce, description, &m_effect, NULL);
		break;
	case 1:
		description = BuildFrictionDescription(m_axisMode, 0.0f);
		g_lastJoystickInputResult = p_device->CreateEffect(GUID_Friction, description, &m_effect, NULL);
		break;
	case 2:
		description = BuildSpringDescription(m_axisMode, 0.0f);
		g_lastJoystickInputResult = p_device->CreateEffect(GUID_Spring, description, &m_effect, NULL);
		break;
	case 3:
		description = BuildSineDescription(m_axisMode, 1.0f, 0.0f);
		g_lastJoystickInputResult = p_device->CreateEffect(GUID_Sine, description, &m_effect, NULL);
		break;
	case 4:
		description = BuildSquareDescription(m_axisMode, 1.0f, 0.0f, 0.5f);
		g_lastJoystickInputResult = p_device->CreateEffect(GUID_Square, description, &m_effect, NULL);
		break;
	case 5:
		description = BuildEnvelopedConstantDescription(m_axisMode, 0.0f, 0.0f);
		g_lastJoystickInputResult = p_device->CreateEffect(GUID_ConstantForce, description, &m_effect, NULL);
		break;
	case 6:
		description = BuildEnvelopedSineDescription(m_axisMode, 0.0f, 0.0f, 0.0f);
		g_lastJoystickInputResult = p_device->CreateEffect(GUID_Sine, description, &m_effect, NULL);
		break;
	default:
		break;
	}
}

// STUB: LEGORACERS2 0x004ed520
void JoystickEffect::SetConstantForce(LegoFloat p_x, LegoFloat p_y)
{
	g_lastJoystickInputResult = m_effect->SetParameters(
		BuildConstantForceDescription(m_axisMode, p_x, p_y),
		DIEP_START | DIEP_TYPESPECIFICPARAMS | DIEP_DIRECTION
	);
	if (g_lastJoystickInputResult != DI_OK) {
		CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputEffect_SetParameters");
	}
	else {
		m_isPlaying = 1;
	}
}

// STUB: LEGORACERS2 0x004ed580
void JoystickEffect::SetSpringCoefficient(LegoFloat p_coefficient)
{
	if (m_effect != NULL) {
		g_lastJoystickInputResult = m_effect->SetParameters(
			BuildSpringDescription(m_axisMode, p_coefficient),
			DIEP_START | DIEP_TYPESPECIFICPARAMS
		);
		if (g_lastJoystickInputResult != DI_OK) {
			CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputEffect_SetParameters");
		}
		else {
			m_isPlaying = 1;
		}
	}
}

// STUB: LEGORACERS2 0x004ed5e0
void JoystickEffect::SetFrictionCoefficient(LegoFloat p_coefficient)
{
	if (m_effect != NULL) {
		g_lastJoystickInputResult = m_effect->SetParameters(
			BuildFrictionDescription(m_axisMode, p_coefficient),
			DIEP_START | DIEP_TYPESPECIFICPARAMS
		);
		if (g_lastJoystickInputResult != DI_OK) {
			CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputEffect_SetParameters");
		}
		else {
			m_isPlaying = 1;
		}
	}
}

// STUB: LEGORACERS2 0x004ed640
void JoystickEffect::SetEnvelopeConstantForce(LegoFloat p_x, LegoFloat p_y)
{
	if (m_effect != NULL) {
		g_lastJoystickInputResult = m_effect->SetParameters(
			BuildEnvelopedConstantDescription(m_axisMode, p_x, p_y),
			DIEP_START | DIEP_DURATION | DIEP_TYPESPECIFICPARAMS | DIEP_DIRECTION
		);
		if (g_lastJoystickInputResult != DI_OK) {
			CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputEffect_SetParameters");
		}
		else {
			m_isPlaying = 1;
		}
	}
}

// STUB: LEGORACERS2 0x004ed6a0
void JoystickEffect::SetPeriodicParameters(LegoFloat p_frequency, LegoFloat p_magnitude)
{
	if (m_effect != NULL) {
		g_lastJoystickInputResult = m_effect->SetParameters(
			BuildSineDescription(m_axisMode, p_frequency, p_magnitude),
			DIEP_START | DIEP_TYPESPECIFICPARAMS
		);
		if (g_lastJoystickInputResult != DI_OK) {
			CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputEffect_SetParameters");
		}
		else {
			m_isPlaying = 1;
		}
	}
}

// STUB: LEGORACERS2 0x004ed700
void JoystickEffect::SetEnvelopePeriodicParameters(LegoFloat p_frequency, LegoFloat p_magnitude, LegoFloat p_duration)
{
	if (m_effect != NULL) {
		g_lastJoystickInputResult = m_effect->SetParameters(
			BuildEnvelopedSineDescription(m_axisMode, p_frequency, p_magnitude, p_duration),
			DIEP_START | DIEP_DURATION | DIEP_TYPESPECIFICPARAMS
		);
		if (g_lastJoystickInputResult != DI_OK) {
			CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputEffect_SetParameters");
		}
		else {
			m_isPlaying = 1;
		}
	}
}

// STUB: LEGORACERS2 0x004ed760
void JoystickEffect::SetSquareParameters(LegoFloat p_first, LegoFloat p_second, LegoFloat p_third)
{
	if (m_effect != NULL) {
		// The retail helper at 0x00473140 returns null for this effect descriptor.
		g_lastJoystickInputResult = m_effect->SetParameters(
			BuildSquareDescription(m_axisMode, p_first, p_second, p_third),
			DIEP_START | DIEP_TYPESPECIFICPARAMS | DIEP_DIRECTION
		);
		if (g_lastJoystickInputResult != DI_OK) {
			CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputEffect_SetParameters");
		}
		else {
			m_isPlaying = 1;
		}
	}
}

// STUB: LEGORACERS2 0x004ed7c0
LegoBool32 JoystickEffect::Stop()
{
	if (m_effect == NULL || m_isPlaying == 0) {
		return FALSE;
	}

	g_lastJoystickInputResult = m_effect->Stop();
	m_isPlaying = 0;
	m_unk0xe0 = 0;
	return g_lastJoystickInputResult == DI_OK;
}

// FUNCTION: LEGORACERS2 0x004ed810
LegoBool32 JoystickEffect::Release()
{
	if (m_effect != NULL) {
		m_effect->Unload();
		m_effect->Release();
		m_effect = NULL;
	}
	m_device = NULL;
	m_isPlaying = 0;
	m_axisMode = 0;
	return TRUE;
}
