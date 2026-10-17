#ifndef LR2_INPUTDEVICE_H
#define LR2_INPUTDEVICE_H

#include "decomp.h"
#include "directx8.h"
#include "inputdispatch.h"
#include "types.h"

#include <stddef.h>

struct InputDeviceRecord {
	void LoadDeviceInfo();
	LegoFloat GetButtonValue(LegoU32 p_buttonIndex);

	const InputDeviceDispatch* m_vtable; // 0x00
	LegoU32 m_isAvailable;               // 0x04
	LegoU32 m_deviceType;                // 0x08
	LegoChar m_productName[0x80];        // 0x0c
	LPDIRECTINPUTDEVICE8A m_device;      // 0x8c
	DIDEVCAPS m_capabilities;            // 0x90
};

DECOMP_SIZE_ASSERT(InputDeviceRecord, 0xbc);
DECOMP_STATIC_ASSERT(offsetof(InputDeviceRecord, m_isAvailable) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(InputDeviceRecord, m_deviceType) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(InputDeviceRecord, m_device) == 0x8c);
DECOMP_SIZE_ASSERT(DIDEVCAPS, 0x2c);
DECOMP_STATIC_ASSERT(offsetof(InputDeviceRecord, m_capabilities) == 0x90);
DECOMP_STATIC_ASSERT(offsetof(DIDEVCAPS, dwButtons) == 0x10);

extern HRESULT g_lastDirectInputResult;
extern HRESULT g_lastMouseInputResult;
extern HRESULT g_lastKeyboardInputResult;

#endif // LR2_INPUTDEVICE_H
