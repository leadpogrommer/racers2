#include "inputjoystick.h"

#include "inputdeviceservice.h"
#include "inputsetup.h"
#include "memoryheap.h"
#include "nativewindow.h"

#include <new.h>
#include <string.h>

// GLOBAL: LEGORACERS2 0x006e47d0
JoystickInputDevice* g_lastJoystickDevice = NULL;

// FUNCTION: LEGORACERS2 0x004ed850
void JoystickInputDevice::Shutdown()
{
	SetForegroundWindow(g_mainWindow);
	Acquire();
	ReleaseEffects();
	SetAutoCenter(TRUE);
	Unacquire();
	if (m_device != NULL) {
		m_device->Release();
		m_device = NULL;
	}
	m_isInitialized = FALSE;
	m_axisCount = 0;
	m_forceFeedbackAxisCount = 0;
	m_autoRangeInitialized = FALSE;
	m_forceFeedbackEnabled = FALSE;
}

// STUB: LEGORACERS2 0x004ed8c0
JoystickInputDevice::JoystickInputDevice()
{
	// The temporary base table has four purecall entries. No dispatch occurs
	// during the member constructors, so only the final derived table is kept.
	m_isAvailable = FALSE;
	m_axisCount = 0;
	m_forceFeedbackAxisCount = 0;
	m_isAcquired = FALSE;
	m_isInitialized = FALSE;
	m_forceFeedbackEnabled = FALSE;
	m_vtable = &g_joystickInputDispatch;
	m_deviceType = 3;
}

// FUNCTION: LEGORACERS2 0x004ed950
void JoystickInputDevice::BindAxes()
{
	m_axisCount = 6;
	for (LegoS32 i = 0; i < m_axisCount; i++) {
		m_axes[i].Bind(reinterpret_cast<LegoS32*>(&m_state), i * sizeof(LegoS32), m_device);
	}
}

// STUB: LEGORACERS2 0x004ed9a0
LegoBool32 JoystickInputDevice::Initialize(LPCDIDEVICEINSTANCEA p_instance)
{
	SetForegroundWindow(g_mainWindow);
	if (m_isInitialized != FALSE) {
		m_vtable->cleanup(this, NULL);
	}
	m_deviceType = 3;
	m_isInitialized = TRUE;
	m_autoRangeInitialized = FALSE;
	m_forceFeedbackEnabled = FALSE;
	LPDIRECTINPUTDEVICE8A device;
	g_lastJoystickInputResult =
		g_inputDeviceService.m_directInput->CreateDevice(p_instance->guidInstance, &device, NULL);
	CheckDirectInputResult(device, g_lastJoystickInputResult, "CreateDevice");
	m_device = device;
	g_lastJoystickInputResult = m_device->SetDataFormat(&c_dfDIJoystick);
	CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputDevice8_SetDataFormat");
	g_lastJoystickInputResult = m_device->SetCooperativeLevel(g_mainWindow, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputDevice8_SetCooperativeLevel");
	memset(&m_capabilities, 0, sizeof(m_capabilities));
	m_capabilities.dwSize = sizeof(m_capabilities);
	g_lastJoystickInputResult = m_device->GetCapabilities(&m_capabilities);
	CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputDevice8_GetCapabilities");
	m_axisCount = 0;
	m_forceFeedbackAxisCount = 0;
	Acquire();
	g_lastJoystickInputResult = m_device->EnumObjects(EnumerateJoystickAxisObject, this, DIDFT_AXIS);
	CheckDirectInputResult(m_device, g_lastJoystickInputResult, "IDirectInputDevice8_EnumObjects");
	g_lastJoystickDevice = this;
	BindAxes();
	if (m_capabilities.dwFlags & DIDC_FORCEFEEDBACK) {
		InitializeEffects(this);
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004edb50
BOOL CALLBACK EnumerateJoystickDevice(LPCDIDEVICEINSTANCEA p_instance, LPVOID)
{
	void* storage = AllocateHeapMemory(sizeof(JoystickInputDevice));
	JoystickInputDevice* device = storage ? new (storage) JoystickInputDevice : NULL;
	if (device->Initialize(p_instance) == TRUE) {
		g_inputDeviceService.RegisterDevice(device);
		return TRUE;
	}
	if (device != NULL) {
		// Retail destroys all 32 embedded effects in reverse order. Their
		// destructor is the shared empty target 0x00479030, not COM cleanup.
		device->~JoystickInputDevice();
		FreeHeapMemory(device);
	}
	return TRUE;
}

// FUNCTION: LEGORACERS2 0x004edbd0
BOOL CALLBACK EnumerateJoystickAxisObject(LPCDIDEVICEOBJECTINSTANCEA p_object, LPVOID p_context)
{
	return static_cast<JoystickInputDevice*>(p_context)->CountForceFeedbackAxis(p_object);
}

// FUNCTION: LEGORACERS2 0x004edd20
LegoBool32 JoystickInputDevice::CountForceFeedbackAxis(LPCDIDEVICEOBJECTINSTANCEA p_object)
{
	if (m_axisCount >= 32) {
		return FALSE;
	}
	if (p_object->dwFlags & DIDOI_FFACTUATOR) {
		m_forceFeedbackAxisCount++;
	}
	return TRUE;
}
