#include "core/application/applicationpolicy.h"
#include "input/devices/inputkeyboard.h"
#include "input/devices/inputmouse.h"

// These are the same shared leaf targets used by the retail application tables.
void ApplicationNoOp();
void WINAPI ApplicationTwoArgumentNoOp(LegoU32, LegoU32);

// FUNCTION: LEGORACERS2 0x004aefe0
void WINAPI InputThreeArgumentNoOp(LegoU32, LegoU32, LegoU32)
{
}

// FUNCTION: LEGORACERS2 0x004aeff0
LegoFloat WINAPI InputZeroAxis(LegoU32)
{
	return 0.0f;
}

LegoBool32 __fastcall InvokeMouseReadState(InputDeviceRecord* p_device, void*, LegoU32 p_mode)
{
	return static_cast<MouseInputDevice*>(p_device)->ReadState(p_mode);
}

LegoBool32 __fastcall InvokeMouseButton(InputDeviceRecord* p_device, void*, LegoU32 p_index)
{
	return static_cast<MouseInputDevice*>(p_device)->IsButtonPressed(p_index);
}

LegoFloat __fastcall InvokeMouseAxis(InputDeviceRecord* p_device, void*, LegoU32 p_index)
{
	return static_cast<MouseInputDevice*>(p_device)->GetAxisValue(p_index);
}

LegoBool32 __fastcall InvokeKeyboardReadState(InputDeviceRecord* p_device, void*, LegoU32 p_mode)
{
	return static_cast<KeyboardInputDevice*>(p_device)->ReadState(p_mode);
}

LegoBool32 __fastcall InvokeKeyboardKey(InputDeviceRecord* p_device, void*, LegoU32 p_index)
{
	return static_cast<KeyboardInputDevice*>(p_device)->IsKeyPressed(p_index);
}

LegoFloat __fastcall InvokeInputButtonValue(InputDeviceRecord* p_device, void*, LegoU32 p_index)
{
	return p_device->GetButtonValue(p_index);
}

// Retail 0x00535928 and 0x0053598c: all 25 entries, with explicit ECX/stack
// bridges for recovered member bodies. The wiring checker validates every
// original target and rebuilt pointer independently of function matching.
const InputDeviceDispatch g_mouseInputDispatch = {
	InvokeMouseReadState,                                                            // 0x00
	InvokeMouseButton,                                                               // 0x04
	InvokeInputButtonValue,                                                          // 0x08
	InvokeMouseAxis,                                                                 // 0x0c
	reinterpret_cast<InputDeviceDispatch::Predicate>(IsApplicationDevelopmentMode),  // 0x10
	reinterpret_cast<InputDeviceDispatch::Predicate>(IsApplicationDevelopmentMode),  // 0x14
	reinterpret_cast<InputDeviceDispatch::OneArgument>(OnApplicationDiagnostic),     // 0x18
	reinterpret_cast<InputDeviceDispatch::Predicate>(IsApplicationDevelopmentMode),  // 0x1c
	reinterpret_cast<InputDeviceDispatch::Action>(ApplicationNoOp),                  // 0x20
	reinterpret_cast<InputDeviceDispatch::Predicate>(IsApplicationDevelopmentMode),  // 0x24
	reinterpret_cast<InputDeviceDispatch::TwoArguments>(ApplicationTwoArgumentNoOp), // 0x28
	reinterpret_cast<InputDeviceDispatch::OneArgument>(OnApplicationDiagnostic),     // 0x2c
	reinterpret_cast<InputDeviceDispatch::OneArgument>(OnApplicationDiagnostic),     // 0x30
	reinterpret_cast<InputDeviceDispatch::OneArgument>(OnApplicationDiagnostic),     // 0x34
	reinterpret_cast<InputDeviceDispatch::TwoArguments>(ApplicationTwoArgumentNoOp), // 0x38
	reinterpret_cast<InputDeviceDispatch::ThreeArguments>(InputThreeArgumentNoOp),   // 0x3c
	reinterpret_cast<InputDeviceDispatch::ThreeArguments>(InputThreeArgumentNoOp),   // 0x40
	reinterpret_cast<InputDeviceDispatch::Action>(ApplicationNoOp),                  // 0x44
	reinterpret_cast<InputDeviceDispatch::Action>(ApplicationNoOp),                  // 0x48
	reinterpret_cast<InputDeviceDispatch::Action>(ApplicationNoOp),                  // 0x4c
	reinterpret_cast<InputDeviceDispatch::Action>(ApplicationNoOp),                  // 0x50
	reinterpret_cast<InputDeviceDispatch::Action>(ApplicationNoOp),                  // 0x54
	reinterpret_cast<InputDeviceDispatch::Action>(ApplicationNoOp),                  // 0x58
	reinterpret_cast<InputDeviceDispatch::Action>(ApplicationNoOp),                  // 0x5c
	reinterpret_cast<InputDeviceDispatch::Action>(ApplicationNoOp),                  // 0x60
};

const InputDeviceDispatch g_keyboardInputDispatch = {
	InvokeKeyboardReadState,                                                         // 0x00
	InvokeKeyboardKey,                                                               // 0x04
	InvokeInputButtonValue,                                                          // 0x08
	reinterpret_cast<InputDeviceDispatch::IndexedFloat>(InputZeroAxis),              // 0x0c
	reinterpret_cast<InputDeviceDispatch::Predicate>(IsApplicationDevelopmentMode),  // 0x10
	reinterpret_cast<InputDeviceDispatch::Predicate>(IsApplicationDevelopmentMode),  // 0x14
	reinterpret_cast<InputDeviceDispatch::OneArgument>(OnApplicationDiagnostic),     // 0x18
	reinterpret_cast<InputDeviceDispatch::Predicate>(IsApplicationDevelopmentMode),  // 0x1c
	reinterpret_cast<InputDeviceDispatch::Action>(ApplicationNoOp),                  // 0x20
	reinterpret_cast<InputDeviceDispatch::Predicate>(IsApplicationDevelopmentMode),  // 0x24
	reinterpret_cast<InputDeviceDispatch::TwoArguments>(ApplicationTwoArgumentNoOp), // 0x28
	reinterpret_cast<InputDeviceDispatch::OneArgument>(OnApplicationDiagnostic),     // 0x2c
	reinterpret_cast<InputDeviceDispatch::OneArgument>(OnApplicationDiagnostic),     // 0x30
	reinterpret_cast<InputDeviceDispatch::OneArgument>(OnApplicationDiagnostic),     // 0x34
	reinterpret_cast<InputDeviceDispatch::TwoArguments>(ApplicationTwoArgumentNoOp), // 0x38
	reinterpret_cast<InputDeviceDispatch::ThreeArguments>(InputThreeArgumentNoOp),   // 0x3c
	reinterpret_cast<InputDeviceDispatch::ThreeArguments>(InputThreeArgumentNoOp),   // 0x40
	reinterpret_cast<InputDeviceDispatch::Action>(ApplicationNoOp),                  // 0x44
	reinterpret_cast<InputDeviceDispatch::Action>(ApplicationNoOp),                  // 0x48
	reinterpret_cast<InputDeviceDispatch::Action>(ApplicationNoOp),                  // 0x4c
	reinterpret_cast<InputDeviceDispatch::Action>(ApplicationNoOp),                  // 0x50
	reinterpret_cast<InputDeviceDispatch::Action>(ApplicationNoOp),                  // 0x54
	reinterpret_cast<InputDeviceDispatch::Action>(ApplicationNoOp),                  // 0x58
	reinterpret_cast<InputDeviceDispatch::Action>(ApplicationNoOp),                  // 0x5c
	reinterpret_cast<InputDeviceDispatch::Action>(ApplicationNoOp),                  // 0x60
};

// These constructors are inlined in retail 0x004aed30. Device pointers,
// capability blocks, names and input state remain uninitialized until the
// corresponding DirectInput setup/read, as in the original allocation path.
MouseInputDevice::MouseInputDevice()
{
	m_vtable = &g_mouseInputDispatch;
	m_isAvailable = FALSE;
	m_deviceType = 2;
}

KeyboardInputDevice::KeyboardInputDevice()
{
	m_vtable = &g_keyboardInputDispatch;
	m_isAvailable = FALSE;
	m_deviceType = 1;
}

DECOMP_SIZE_ASSERT(MouseInputDevice, 0xcc);
DECOMP_SIZE_ASSERT(KeyboardInputDevice, 0x1bc);
