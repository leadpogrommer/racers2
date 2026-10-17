#include "inputdeviceservice.h"
#include "inputformats.h"
#include "inputjoystick.h"
#include "inputkeyboard.h"
#include "inputmouse.h"
#include "inputsetup.h"
#include "memoryheap.h"
#include "nativewindow.h"
#include "startup.h"

#include <new.h>

LegoBool32 __fastcall InvokeInputServicePoll(InputDeviceService* p_service, void*, LegoU32 p_state)
{
	return p_service->PollDevices(p_state);
}
LegoBool32 __fastcall InvokeInputServiceInitialize(InputDeviceService* p_service, void*)
{
	return p_service->Initialize();
}
void __fastcall InvokeInputServiceShutdown(InputDeviceService* p_service, void*)
{
	p_service->Shutdown();
}
void __fastcall InvokeInputServiceRefresh(InputDeviceService* p_service, void*)
{
	p_service->RefreshDevices();
}
InputDeviceRecord* __fastcall InvokeInputServiceFind(
	InputDeviceService* p_service,
	void*,
	LegoU32 p_index,
	LegoU32 p_type
)
{
	return p_service->FindDeviceByType(p_index, p_type);
}

// Retail 0x00535914. These five bridges preserve the observed game ABI.
const InputServiceDispatch g_inputServiceDispatch = {
	InvokeInputServicePoll,
	InvokeInputServiceInitialize,
	InvokeInputServiceShutdown,
	InvokeInputServiceRefresh,
	InvokeInputServiceFind,
};

// GLOBAL: LEGORACERS2 0x00653080
InputDeviceService g_inputDeviceService;

// GLOBAL: LEGORACERS2 0x00549e80
InputDeviceService* g_inputService = &g_inputDeviceService;

// Retail dynamic initializer 0x004aece0 writes only these three fields.
InputDeviceService::InputDeviceService()
{
	m_vtable = &g_inputServiceDispatch;
	m_directInput = NULL;
	m_deviceCount = 0;
}

// STUB: LEGORACERS2 0x004aed30
LegoBool32 InputDeviceService::Initialize()
{
	SetActiveWindow(g_mainWindow);
	g_lastDirectInputResult =
		DirectInput8Create(g_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8A, (void**) &m_directInput, NULL);
	if (g_lastDirectInputResult != DI_OK) {
		return FALSE;
	}

	void* storage = AllocateHeapMemory(sizeof(KeyboardInputDevice));
	m_firstDevice = storage ? new (storage) KeyboardInputDevice : NULL;
	LPDIRECTINPUTDEVICE8A device;
	g_lastDirectInputResult = m_directInput->CreateDevice(GUID_SysKeyboard, &device, NULL);
	m_firstDevice->m_device = device;
	CheckDirectInputResult(device, g_lastDirectInputResult, "CreateDevice");
	g_lastDirectInputResult =
		m_firstDevice->m_device->SetCooperativeLevel(g_mainWindow, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	CheckDirectInputResult(device, g_lastDirectInputResult, "IDirectInputDevice8_SetCooperativeLevel");
	g_lastDirectInputResult = m_firstDevice->m_device->SetDataFormat(&c_dfDIKeyboard);
	CheckDirectInputResult(device, g_lastDirectInputResult, "IDirectInputDevice8_SetDataFormat");
	RegisterDevice(m_firstDevice);

	storage = AllocateHeapMemory(sizeof(MouseInputDevice));
	m_secondDevice = storage ? new (storage) MouseInputDevice : NULL;
	g_lastDirectInputResult = m_directInput->CreateDevice(GUID_SysMouse, &device, NULL);
	m_secondDevice->m_device = device;
	CheckDirectInputResult(device, g_lastDirectInputResult, "SetDeviceObject");
	g_lastDirectInputResult =
		m_secondDevice->m_device->SetCooperativeLevel(g_mainWindow, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	CheckDirectInputResult(device, g_lastDirectInputResult, "IDirectInputDevice8_SetCooperativeLevel");
	g_lastDirectInputResult = m_secondDevice->m_device->SetDataFormat(&g_mouseInputDataFormat);
	CheckDirectInputResult(device, g_lastDirectInputResult, "IDirectInputDevice8_SetDataFormat");
	RegisterDevice(m_secondDevice);
	g_lastDirectInputResult =
		m_secondDevice->m_device->EnumObjects(EnumerateMouseAxisObject, m_secondDevice, DIDFT_ALL);
	CheckDirectInputResult(device, g_lastDirectInputResult, "IDirectInputDevice8_EnumObjects");

	g_lastDirectInputResult =
		m_directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumerateJoystickDevice, NULL, DIEDFL_ATTACHEDONLY);
	CheckDirectInputResult(device, g_lastDirectInputResult, "IDirectInput_EnumDevices");
	m_firstDevice->m_isAvailable = TRUE;
	m_secondDevice->m_isAvailable = TRUE;
	LegoU32 index = 0;
	InputDeviceRecord* record = g_inputService->m_vtable->findDevice(g_inputService, NULL, index, 3);
	while (record != NULL) {
		record->m_isAvailable = TRUE;
		record = g_inputService->m_vtable->findDevice(g_inputService, NULL, ++index, 3);
	}
	AcquireAvailableDevices();
	return TRUE;
}
