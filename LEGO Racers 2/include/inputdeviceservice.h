#ifndef LR2_INPUTDEVICESERVICE_H
#define LR2_INPUTDEVICESERVICE_H

#include "inputdevice.h"

class InputDeviceService {
public:
	void AcquireAvailableDevices();
	InputDeviceRecord* FindDeviceByType(LegoU32 p_index, LegoU32 p_type);

	const LegoU32* m_vtable;           // 0x00
	LPDIRECTINPUT8A m_directInput;     // 0x04
	LegoU32 m_deviceCount;             // 0x08
	InputDeviceRecord* m_devices[32];  // 0x0c
	InputDeviceRecord* m_firstDevice;  // 0x8c
	InputDeviceRecord* m_secondDevice; // 0x90
};

DECOMP_STATIC_ASSERT(offsetof(InputDeviceService, m_directInput) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(InputDeviceService, m_deviceCount) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(InputDeviceService, m_devices) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(InputDeviceService, m_firstDevice) == 0x8c);
DECOMP_STATIC_ASSERT(offsetof(InputDeviceService, m_secondDevice) == 0x90);

#endif // LR2_INPUTDEVICESERVICE_H
