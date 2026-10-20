#include "input/service/inputdeviceservice.h"
#include "memory/memoryheap.h"

// STUB: LEGORACERS2 0x004af000
LegoBool32 InputDeviceService::PollDevices(LegoU32 p_inputState)
{
	for (LegoS32 i = 0; i < (LegoS32) m_deviceCount; i++) {
		InputDeviceRecord* device = m_devices[i];
		if (device->m_vtable->isActive(device, NULL)) {
			device = m_devices[i];
			device->m_vtable->performAction(device, NULL, p_inputState);
		}
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004af050
void InputDeviceService::Shutdown()
{
	for (LegoU32 i = 0; i < m_deviceCount; i++) {
		InputDeviceRecord* device = m_devices[i];
		device->m_vtable->cleanup(device, NULL);
		FreeHeapMemory(m_devices[i]);
	}
	m_directInput->Release();
}

// STUB: LEGORACERS2 0x004af170
void InputDeviceService::RefreshDevices()
{
	for (LegoU32 i = 0; i < m_deviceCount; i++) {
		InputDeviceRecord* device = m_devices[i];
		if (device->m_isAvailable) {
			device->m_vtable->readState(device, NULL, 1);
		}
	}
}
