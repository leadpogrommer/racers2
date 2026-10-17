#ifndef LR2_INPUTDEVICESERVICE_H
#define LR2_INPUTDEVICESERVICE_H

#include "inputdevice.h"

class InputDeviceService;

struct InputServiceDispatch {
	typedef LegoBool32(__fastcall* Poll)(InputDeviceService*, void*, LegoU32);
	typedef LegoBool32(__fastcall* Initialize)(InputDeviceService*, void*);
	typedef void(__fastcall* Action)(InputDeviceService*, void*);
	typedef InputDeviceRecord*(__fastcall* Find)(InputDeviceService*, void*, LegoU32, LegoU32);

	Poll poll;
	Initialize initialize;
	Action shutdown;
	Action refresh;
	Find findDevice;
};

DECOMP_SIZE_ASSERT(InputServiceDispatch, 0x14);

class InputDeviceService {
public:
	InputDeviceService();
	LegoBool32 Initialize();
	LegoBool32 PollDevices(LegoU32 p_inputState);
	void Shutdown();
	void RegisterDevice(InputDeviceRecord* p_device);
	void AcquireAvailableDevices();
	void RefreshDevices();
	InputDeviceRecord* FindDeviceByType(LegoU32 p_index, LegoU32 p_type);

	const InputServiceDispatch* m_vtable; // 0x00
	LPDIRECTINPUT8A m_directInput;        // 0x04
	LegoU32 m_deviceCount;                // 0x08
	InputDeviceRecord* m_devices[32];     // 0x0c
	InputDeviceRecord* m_firstDevice;     // 0x8c
	InputDeviceRecord* m_secondDevice;    // 0x90
};

extern const InputServiceDispatch g_inputServiceDispatch;
extern InputDeviceService g_inputDeviceService;
extern InputDeviceService* g_inputService;

DECOMP_STATIC_ASSERT(offsetof(InputDeviceService, m_directInput) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(InputDeviceService, m_deviceCount) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(InputDeviceService, m_devices) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(InputDeviceService, m_firstDevice) == 0x8c);
DECOMP_STATIC_ASSERT(offsetof(InputDeviceService, m_secondDevice) == 0x90);

#endif // LR2_INPUTDEVICESERVICE_H
