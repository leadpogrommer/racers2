#include "core/application/applicationpolicy.h"
#include "input/devices/inputjoystick.h"

LegoBool32 __fastcall InvokeJoystickReadState(InputDeviceRecord* p_device, void*, LegoU32 p_mode)
{
	return static_cast<JoystickInputDevice*>(p_device)->ReadState(p_mode);
}
LegoBool32 __fastcall InvokeJoystickButton(InputDeviceRecord* p_device, void*, LegoU32 p_index)
{
	return static_cast<JoystickInputDevice*>(p_device)->IsButtonPressed(p_index);
}
LegoFloat __fastcall InvokeJoystickButtonValue(InputDeviceRecord* p_device, void*, LegoU32 p_index)
{
	return p_device->GetButtonValue(p_index);
}
LegoFloat __fastcall InvokeJoystickAxis(InputDeviceRecord* p_device, void*, LegoS32 p_index)
{
	return static_cast<JoystickInputDevice*>(p_device)->GetAxisValue(p_index);
}
LegoS32 __fastcall InvokeJoystickAxisCount(InputDeviceRecord* p_device, void*)
{
	return static_cast<JoystickInputDevice*>(p_device)->GetAxisCount();
}
void __fastcall InvokeJoystickEnableFeedback(InputDeviceRecord* p_device, void*, LegoU32 p_enabled)
{
	static_cast<JoystickInputDevice*>(p_device)->EnableForceFeedback(p_enabled);
}
void __fastcall InvokeJoystickShutdown(InputDeviceRecord* p_device, void*)
{
	static_cast<JoystickInputDevice*>(p_device)->Shutdown();
}
LegoBool32 __fastcall InvokeJoystickHasFeedbackAxes(InputDeviceRecord* p_device, void*)
{
	return static_cast<JoystickInputDevice*>(p_device)->HasForceFeedbackAxes();
}
void __fastcall InvokeJoystickConstantForce(InputDeviceRecord* p_device, void*, LegoU32 p_axis, LegoFloat p_force)
{
	static_cast<JoystickInputDevice*>(p_device)->SetConstantForce(p_axis, p_force);
}
void __fastcall InvokeJoystickSpring(InputDeviceRecord* p_device, void*, LegoFloat p_value)
{
	static_cast<JoystickInputDevice*>(p_device)->SetSpring(p_value);
}
void __fastcall InvokeJoystickFriction(InputDeviceRecord* p_device, void*, LegoFloat p_value)
{
	static_cast<JoystickInputDevice*>(p_device)->SetFriction(p_value);
}
void __fastcall InvokeJoystickSine(InputDeviceRecord* p_device, void*, LegoFloat p_first, LegoFloat p_second)
{
	static_cast<JoystickInputDevice*>(p_device)->SetSine(p_first, p_second);
}
void __fastcall InvokeJoystickEnvelopedConstant(InputDeviceRecord* p_device, void*, LegoFloat p_value)
{
	static_cast<JoystickInputDevice*>(p_device)->SetEnvelopedConstantForce(p_value);
}
void __fastcall InvokeJoystickSquare(
	InputDeviceRecord* p_device,
	void*,
	LegoFloat p_first,
	LegoFloat p_second,
	LegoFloat p_third
)
{
	static_cast<JoystickInputDevice*>(p_device)->SetSquare(p_first, p_second, p_third);
}
void __fastcall InvokeJoystickEnvelopedSine(
	InputDeviceRecord* p_device,
	void*,
	LegoFloat p_first,
	LegoFloat p_second,
	LegoFloat p_third
)
{
	static_cast<JoystickInputDevice*>(p_device)->SetEnvelopedSine(p_first, p_second, p_third);
}
void __fastcall InvokeJoystickStopAll(InputDeviceRecord* p_device, void*)
{
	static_cast<JoystickInputDevice*>(p_device)->StopAllEffects();
}
void __fastcall InvokeJoystickStopConstant(InputDeviceRecord* p_device, void*)
{
	static_cast<JoystickInputDevice*>(p_device)->StopConstantForce();
}
void __fastcall InvokeJoystickStopSpring(InputDeviceRecord* p_device, void*)
{
	static_cast<JoystickInputDevice*>(p_device)->StopSpring();
}
void __fastcall InvokeJoystickStopFriction(InputDeviceRecord* p_device, void*)
{
	static_cast<JoystickInputDevice*>(p_device)->StopFriction();
}
void __fastcall InvokeJoystickStopSine(InputDeviceRecord* p_device, void*)
{
	static_cast<JoystickInputDevice*>(p_device)->StopSine();
}
void __fastcall InvokeJoystickStopSquare(InputDeviceRecord* p_device, void*)
{
	static_cast<JoystickInputDevice*>(p_device)->StopSquare();
}
void __fastcall InvokeJoystickStopEnvelopedConstant(InputDeviceRecord* p_device, void*)
{
	static_cast<JoystickInputDevice*>(p_device)->StopEnvelopedConstantForce();
}
void __fastcall InvokeJoystickStopEnvelopedSine(InputDeviceRecord* p_device, void*)
{
	static_cast<JoystickInputDevice*>(p_device)->StopEnvelopedSine();
}

// Retail 0x00536e70. The casts preserve the original stack argument bits;
// float arguments occupy the same DWORD slots as the base's untyped no-ops.
const InputDeviceDispatch g_joystickInputDispatch = {
	InvokeJoystickReadState,                                                             // 0x00
	InvokeJoystickButton,                                                                // 0x04
	InvokeJoystickButtonValue,                                                           // 0x08
	reinterpret_cast<InputDeviceDispatch::IndexedFloat>(InvokeJoystickAxis),             // 0x0c
	reinterpret_cast<InputDeviceDispatch::Predicate>(IsApplicationDevelopmentMode),      // 0x10
	reinterpret_cast<InputDeviceDispatch::Predicate>(InvokeJoystickAxisCount),           // 0x14
	InvokeJoystickEnableFeedback,                                                        // 0x18
	reinterpret_cast<InputDeviceDispatch::Predicate>(IsApplicationDevelopmentMode),      // 0x1c
	InvokeJoystickShutdown,                                                              // 0x20
	InvokeJoystickHasFeedbackAxes,                                                       // 0x24
	reinterpret_cast<InputDeviceDispatch::TwoArguments>(InvokeJoystickConstantForce),    // 0x28
	reinterpret_cast<InputDeviceDispatch::OneArgument>(InvokeJoystickSpring),            // 0x2c
	reinterpret_cast<InputDeviceDispatch::OneArgument>(InvokeJoystickFriction),          // 0x30
	reinterpret_cast<InputDeviceDispatch::OneArgument>(InvokeJoystickEnvelopedConstant), // 0x34
	reinterpret_cast<InputDeviceDispatch::TwoArguments>(InvokeJoystickSine),             // 0x38
	reinterpret_cast<InputDeviceDispatch::ThreeArguments>(InvokeJoystickSquare),         // 0x3c
	reinterpret_cast<InputDeviceDispatch::ThreeArguments>(InvokeJoystickEnvelopedSine),  // 0x40
	InvokeJoystickStopAll,                                                               // 0x44
	InvokeJoystickStopConstant,                                                          // 0x48
	InvokeJoystickStopSpring,                                                            // 0x4c
	InvokeJoystickStopFriction,                                                          // 0x50
	InvokeJoystickStopSine,                                                              // 0x54
	InvokeJoystickStopSquare,                                                            // 0x58
	InvokeJoystickStopEnvelopedConstant,                                                 // 0x5c
	InvokeJoystickStopEnvelopedSine,                                                     // 0x60
};
