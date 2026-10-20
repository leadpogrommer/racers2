#ifndef LR2_STEREOAUDIOPROVIDER_H
#define LR2_STEREOAUDIOPROVIDER_H

#include "audio/service/audioservice.h"

class StereoDirectSoundProvider {
public:
	StereoDirectSoundProvider(HWND p_window);
	void Initialize(HWND p_window);
	void ReleaseDeviceInterfaces();

	const AudioBackendStateDispatch* m_vtable; // 0x00
	IDirectSound8* m_device;                   // 0x04
	IDirectSoundBuffer* m_primaryBuffer;       // 0x08
	LegoU32 m_initializationStatus;            // 0x0c
};

DECOMP_STATIC_ASSERT(offsetof(StereoDirectSoundProvider, m_device) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(StereoDirectSoundProvider, m_initializationStatus) == 0x0c);
DECOMP_SIZE_ASSERT(StereoDirectSoundProvider, 0x10);

extern const AudioBackendStateDispatch g_stereoDirectSoundProviderDispatch;

void* ConstructStereoAudioProvider(void* p_storage, HWND p_window);

#endif // LR2_STEREOAUDIOPROVIDER_H
