#include "input/service/inputsetup.h"

#include "input/devices/inputdevice.h"
#include "input/devices/inputmouse.h"
#include "input/service/inputdeviceservice.h"

#include <stddef.h>
#include <string.h>

DECOMP_SIZE_ASSERT(DIDEVICEINSTANCEA, 0x244);
DECOMP_STATIC_ASSERT(offsetof(DIDEVICEINSTANCEA, tszProductName) == 0x12c);
DECOMP_STATIC_ASSERT(offsetof(DIDEVICEOBJECTINSTANCEA, dwOfs) == 0x14);
DECOMP_STATIC_ASSERT(offsetof(DIDEVICEOBJECTINSTANCEA, dwType) == 0x18);
DECOMP_SIZE_ASSERT(DIPROPRANGE, 0x18);

static const LegoChar kGetCapabilitiesMessage[] = "IDirectInputDevice8_GetCapabilities";

// FUNCTION: LEGORACERS2 0x004aed00
LegoBool32
CheckDirectInputResult(LPDIRECTINPUTDEVICE8A p_device, HRESULT p_result, const LegoChar* /*p_ignoredMessage*/)
{
	if (p_result == DI_OK) {
		return TRUE;
	}

	if (p_device != NULL) {
		p_device->GetForceFeedbackState(reinterpret_cast<LPDWORD>(&p_result));
	}
	return FALSE;
}

// STUB: LEGORACERS2 0x004af090
void InputDeviceService::RegisterDevice(InputDeviceRecord* p_device)
{
	memset(&p_device->m_capabilities, 0, sizeof(p_device->m_capabilities));
	p_device->m_capabilities.dwSize = sizeof(p_device->m_capabilities);
	g_lastDirectInputResult = p_device->m_device->GetCapabilities(&p_device->m_capabilities);
	CheckDirectInputResult(p_device->m_device, g_lastDirectInputResult, kGetCapabilitiesMessage);
	p_device->LoadDeviceInfo();
	m_devices[m_deviceCount] = p_device;
	m_deviceCount++;
}

// STUB: LEGORACERS2 0x004af1e0
void InputDeviceRecord::LoadDeviceInfo()
{
	DIDEVICEINSTANCEA info;
	memset(&info, 0, sizeof(info));
	info.dwSize = sizeof(info);
	HRESULT result = m_device->GetDeviceInfo(&info);
	if (result == DI_OK) {
		strcpy(m_productName, info.tszProductName);
	}
	else {
		m_productName[0] = '\0';
	}
}

// FUNCTION: LEGORACERS2 0x004ec3f0
LegoBool32 MouseInputDevice::SetRelativeAxisRange(LPCDIDEVICEOBJECTINSTANCEA p_object)
{
	if ((p_object->dwType & DIDFT_RELAXIS) != 0) {
		DIPROPRANGE range;
		range.diph.dwSize = sizeof(range);
		range.diph.dwHeaderSize = sizeof(range.diph);
		range.diph.dwObj = p_object->dwOfs;
		range.diph.dwHow = DIPH_BYOFFSET;
		range.lMin = -255;
		range.lMax = 255;
		g_lastMouseInputResult = m_device->SetProperty(DIPROP_RANGE, &range.diph);
	}
	return TRUE;
}

// FUNCTION: LEGORACERS2 0x004ec450
BOOL CALLBACK EnumerateMouseAxisObject(LPCDIDEVICEOBJECTINSTANCEA p_object, LPVOID p_context)
{
	return static_cast<MouseInputDevice*>(p_context)->SetRelativeAxisRange(p_object);
}
