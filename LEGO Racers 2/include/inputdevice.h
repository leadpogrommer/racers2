#ifndef LR2_INPUTDEVICE_H
#define LR2_INPUTDEVICE_H

#include "decomp.h"
#include "directx8.h"
#include "types.h"

#include <stddef.h>

struct InputDeviceRecord {
	const LegoU32* m_vtable;          // 0x00
	LegoU32 m_isAvailable;            // 0x04
	LegoU32 m_deviceType;             // 0x08
	LegoChar m_productName[0x80];     // 0x0c
	LPDIRECTINPUTDEVICE8A m_device;   // 0x8c
	undefined m_unk0x90[0xbc - 0x90]; // 0x90, property/configuration scratch area
};

DECOMP_SIZE_ASSERT(InputDeviceRecord, 0xbc);
DECOMP_STATIC_ASSERT(offsetof(InputDeviceRecord, m_isAvailable) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(InputDeviceRecord, m_deviceType) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(InputDeviceRecord, m_device) == 0x8c);

extern HRESULT g_lastDirectInputResult;
extern HRESULT g_lastMouseInputResult;
extern HRESULT g_lastKeyboardInputResult;

#endif // LR2_INPUTDEVICE_H
