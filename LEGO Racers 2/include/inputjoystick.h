#ifndef LR2_INPUTJOYSTICK_H
#define LR2_INPUTJOYSTICK_H

#include "inputdevice.h"
#include "joystickaxis.h"
#include "joystickeffect.h"

struct JoystickInputDevice : InputDeviceRecord {
	JoystickInputDevice();
	LegoBool32 Initialize(LPCDIDEVICEINSTANCEA p_instance);
	void Shutdown();
	void BindAxes();
	LegoBool32 CountForceFeedbackAxis(LPCDIDEVICEOBJECTINSTANCEA p_object);
	LegoBool32 ReadState(LegoU32 p_mode);
	LegoBool32 IsButtonPressed(LegoU32 p_buttonIndex);
	LegoFloat GetAxisValue(LegoS32 p_axisIndex);
	LegoS32 GetAxisCount();
	LegoBool32 HasForceFeedbackAxes();
	LegoBool32 Acquire();
	LegoBool32 Unacquire();
	void UpdateAutoRange();

	void ResetForceFeedback();
	void InitializeEffects(JoystickInputDevice* p_device);
	void ReleaseEffects();
	void SetAutoCenter(LegoBool32 p_enabled);
	void EnableForceFeedback(LegoU32 p_enabled);
	void StopAllEffects();
	void SetConstantForce(LegoU32 p_axis, LegoFloat p_force);
	void SetFriction(LegoFloat p_value);
	void SetSpring(LegoFloat p_value);
	void SetSine(LegoFloat p_first, LegoFloat p_second);
	void SetSquare(LegoFloat p_first, LegoFloat p_second, LegoFloat p_third);
	void SetEnvelopedConstantForce(LegoFloat p_value);
	void SetEnvelopedSine(LegoFloat p_first, LegoFloat p_second, LegoFloat p_third);
	void StopConstantForce();
	void StopFriction();
	void StopSpring();
	void StopSine();
	void StopSquare();
	void StopEnvelopedConstantForce();
	void StopEnvelopedSine();

	JoystickEffect m_effects[32];      // 0x00bc
	JoystickAxis m_axes[32];           // 0x1d3c
	LegoS32 m_axisCount;               // 0x20bc
	LegoS32 m_forceFeedbackAxisCount;  // 0x20c0
	DIJOYSTATE m_state;                // 0x20c4
	LegoBool32 m_isAcquired;           // 0x2114
	LegoBool32 m_isInitialized;        // 0x2118
	LegoBool32 m_autoRangeInitialized; // 0x211c
	LegoBool32 m_forceFeedbackEnabled; // 0x2120
};

DECOMP_SIZE_ASSERT(DIJOYSTATE, 0x50);
DECOMP_SIZE_ASSERT(JoystickInputDevice, 0x2124);
DECOMP_STATIC_ASSERT(offsetof(JoystickInputDevice, m_effects) == 0xbc);
DECOMP_STATIC_ASSERT(offsetof(JoystickInputDevice, m_axes) == 0x1d3c);
DECOMP_STATIC_ASSERT(offsetof(JoystickInputDevice, m_axisCount) == 0x20bc);
DECOMP_STATIC_ASSERT(offsetof(JoystickInputDevice, m_forceFeedbackAxisCount) == 0x20c0);
DECOMP_STATIC_ASSERT(offsetof(JoystickInputDevice, m_state) == 0x20c4);
DECOMP_STATIC_ASSERT(offsetof(JoystickInputDevice, m_isAcquired) == 0x2114);
DECOMP_STATIC_ASSERT(offsetof(JoystickInputDevice, m_isInitialized) == 0x2118);
DECOMP_STATIC_ASSERT(offsetof(JoystickInputDevice, m_autoRangeInitialized) == 0x211c);
DECOMP_STATIC_ASSERT(offsetof(JoystickInputDevice, m_forceFeedbackEnabled) == 0x2120);

extern const InputDeviceDispatch g_joystickInputDispatch;
extern JoystickInputDevice* g_lastJoystickDevice;

BOOL CALLBACK EnumerateJoystickDevice(LPCDIDEVICEINSTANCEA p_instance, LPVOID p_context);
BOOL CALLBACK EnumerateJoystickAxisObject(LPCDIDEVICEOBJECTINSTANCEA p_object, LPVOID p_context);

#endif // LR2_INPUTJOYSTICK_H
