#ifndef LR2_JOYSTICKEFFECT_H
#define LR2_JOYSTICKEFFECT_H

#include "decomp.h"
#include "directx8.h"
#include "types.h"

#include <stddef.h>

class JoystickEffect {
public:
	JoystickEffect();
	JoystickEffect(LPDIRECTINPUTDEVICE8A p_device, LegoU32 p_effectKind, LegoU32 p_axisMode);
	~JoystickEffect();

	void SetConstantForce(LegoFloat p_x, LegoFloat p_y);
	void SetSpringCoefficient(LegoFloat p_coefficient);
	void SetFrictionCoefficient(LegoFloat p_coefficient);
	void SetEnvelopeConstantForce(LegoFloat p_x, LegoFloat p_y);
	void SetPeriodicParameters(LegoFloat p_frequency, LegoFloat p_magnitude);
	void SetEnvelopePeriodicParameters(LegoFloat p_frequency, LegoFloat p_magnitude, LegoFloat p_duration);
	void SetSquareParameters(LegoFloat p_first, LegoFloat p_second, LegoFloat p_third);
	LegoBool32 Stop();
	LegoBool32 Release();

	LPDIRECTINPUTDEVICE8A m_device; // 0x00
	LPDIRECTINPUTEFFECT m_effect;   // 0x04
	LegoU32 m_isPlaying;            // 0x08
	LegoU32 m_axisMode;             // 0x0c
	undefined m_unk0x10[0xd0];      // 0x10
	LegoU32 m_unk0xe0;              // 0xe0
};

DECOMP_SIZE_ASSERT(JoystickEffect, 0xe4);
DECOMP_STATIC_ASSERT(offsetof(JoystickEffect, m_device) == 0x00);
DECOMP_STATIC_ASSERT(offsetof(JoystickEffect, m_effect) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(JoystickEffect, m_isPlaying) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(JoystickEffect, m_axisMode) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(JoystickEffect, m_unk0xe0) == 0xe0);

#endif // LR2_JOYSTICKEFFECT_H
