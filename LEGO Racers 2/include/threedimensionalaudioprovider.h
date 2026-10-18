#ifndef LR2_THREEDIMENSIONALAUDIOPROVIDER_H
#define LR2_THREEDIMENSIONALAUDIOPROVIDER_H

#include "audioservice.h"

class ThreeDimensionalDirectSoundProvider {
public:
	ThreeDimensionalDirectSoundProvider(HWND p_window);
	void Initialize(HWND p_window);
	void ReleaseDeviceInterfaces();

	const AudioBackendStateDispatch* m_vtable; // 0x00
	IDirectSound8* m_device;                   // 0x04
	IDirectSoundBuffer* m_primaryBuffer;       // 0x08
	LegoU32 m_initializationStatus;            // 0x0c
	LegoU32 m_unk0x10;
	IDirectSound3DListener* m_listener; // 0x14
	IUnknown* m_unk0x18;
	IUnknown* m_unk0x1c;
	IUnknown* m_unk0x20;
};

DECOMP_STATIC_ASSERT(offsetof(ThreeDimensionalDirectSoundProvider, m_device) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(ThreeDimensionalDirectSoundProvider, m_initializationStatus) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(ThreeDimensionalDirectSoundProvider, m_listener) == 0x14);
DECOMP_SIZE_ASSERT(ThreeDimensionalDirectSoundProvider, 0x24);

extern const AudioBackendStateDispatch g_threeDimensionalDirectSoundProviderDispatch;

void* ConstructThreeDimensionalAudioProvider(void* p_storage, HWND p_window);

#endif // LR2_THREEDIMENSIONALAUDIOPROVIDER_H
