#ifndef LR2_AUDIOPROVIDER_H
#define LR2_AUDIOPROVIDER_H

#include "decomp.h"
#include "directx8.h"
#include "types.h"

#include <stddef.h>

struct AudioSourceRecord;
typedef IDirectSoundBuffer AudioPlaybackObjectView;
class MonoDirectSoundProvider;
struct AudioBackendStateView;

struct AudioBackendStateDispatch;

class MonoDirectSoundProvider {
public:
	MonoDirectSoundProvider(HWND p_window);
	void Initialize(HWND p_window);
	void ReleaseDeviceInterfaces();
	void ReleasePlaybackObject(AudioPlaybackObjectView** p_object);
	void ReleaseSourcePlaybackBuffer(struct AudioSourceRecord* p_source);

	const AudioBackendStateDispatch* m_vtable; // 0x00, 15-slot provider table
	IDirectSound8* m_device;                   // 0x04
	IDirectSoundBuffer* m_primaryBuffer;       // 0x08; temporary during initialization
	LegoU32 m_initializationStatus;            // 0x0c
};

extern const AudioBackendStateDispatch g_monoDirectSoundProviderDispatch;

DECOMP_STATIC_ASSERT(offsetof(MonoDirectSoundProvider, m_device) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(MonoDirectSoundProvider, m_initializationStatus) == 0x0c);
DECOMP_SIZE_ASSERT(MonoDirectSoundProvider, 0x10);

LegoBool32 __stdcall IsAudioPlaybackObjectActive(AudioPlaybackObjectView* p_object);
LegoBool32 __stdcall StartAudioPlaybackObject(AudioPlaybackObjectView* p_object, LegoU32 p_loop);
LegoBool32 __stdcall StopAudioPlaybackObject(AudioPlaybackObjectView* p_object);
LegoBool32 __stdcall SetAudioPlaybackFrequency(AudioPlaybackObjectView* p_object, LegoS32 p_frequency);
LegoBool32 __stdcall ResetAudioPlaybackPosition(AudioPlaybackObjectView* p_object);

struct AudioBackendStateView;

// Provider-table callback at 0x004efdd0. The target ignores ECX/EDX and
// consumes one explicit source-format pointer from the stack.
LegoU32 __fastcall GetWaveSampleRate(
	AudioBackendStateView* p_provider,
	void* p_edx,
	const void* p_source,
	LegoU32 p_unused
);

// Shared DirectSound setup performed by the two non-3D output providers.
// p_mono selects the mono format used by the fallback provider; false selects
// the stereo format. On failure, the state retains the same device pointer
// state the retail setup path leaves behind after any COM Release calls.
LegoBool32 InitializeDirectSoundOutput(
	IDirectSound8** p_device,
	IDirectSoundBuffer** p_primaryBuffer,
	LegoU32* p_initializationStatus,
	HWND p_window,
	LegoBool32 p_mono
);

#endif // LR2_AUDIOPROVIDER_H
