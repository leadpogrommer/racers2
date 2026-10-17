#ifndef LR2_DIRECTX8_H
#define LR2_DIRECTX8_H

// The game uses the DirectX 8 interfaces. Define the interface versions before
// either the Wine compatibility declarations or a genuine DX8 SDK is included.
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#ifndef DIRECTSOUND_VERSION
#define DIRECTSOUND_VERSION 0x0800
#endif

#include <windows.h>

#if defined(LR2_USE_WINE_DX8)
// Wine's current headers contain a few compiler and Winelib assumptions that
// VC6 does not provide. This narrow bridge leaves the declared COM method lists
// and vtable order intact; it does not define replacement interfaces.
#ifndef __MSABI_LONG
#define __MSABI_LONG(value) value
#endif

#include <multimon.h>

#ifndef DECLARE_INTERFACE_IID_
#define DECLARE_INTERFACE_IID_(iface, base, iid) DECLARE_INTERFACE_(iface, base)
#endif

#ifndef WINELIB_NAME_AW
#define WINELIB_NAME_AW(name) name##A
#endif

#ifndef DECL_WINELIB_TYPE_AW
#define DECL_WINELIB_TYPE_AW(type) typedef WINELIB_NAME_AW(type) type;
#endif

#include <basetsd.h>
#endif

#include <d3d8.h>
#include <dinput.h>
#include <dsound.h>

#endif // LR2_DIRECTX8_H
