#ifndef LR2_INPUTSETUP_H
#define LR2_INPUTSETUP_H

#include "directx8.h"
#include "types.h"

LegoBool32 CheckDirectInputResult(LPDIRECTINPUTDEVICE8A p_device, HRESULT p_result, const LegoChar* p_ignoredMessage);
BOOL CALLBACK EnumerateMouseAxisObject(LPCDIDEVICEOBJECTINSTANCEA p_object, LPVOID p_context);

#endif // LR2_INPUTSETUP_H
