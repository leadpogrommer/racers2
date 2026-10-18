#include "audioprovider.h"
#include "audioservice.h"
#include "memoryheap.h"
#include "stereoaudioprovider.h"
#include "threedimensionalaudioprovider.h"

extern "C" int __cdecl _purecall(void);

// Provider operations whose bodies are shared between the three output
// implementations or reconstructed in the adjacent callback unit.
IDirectSoundBuffer* __fastcall CreatePcmSourceBuffer(AudioBackendStateView*, void*, const void*, LegoU32);
IDirectSoundBuffer* __fastcall CreateThreeDimensionalSourceBuffer(AudioBackendStateView*, void*, const void*, LegoU32);
void __fastcall ReleaseStereoSourceBuffer(AudioBackendStateView*, void*, AudioSourceRecord*);
void __fastcall ReleaseThreeDimensionalSourceBuffer(AudioBackendStateView*, void*, AudioSourceRecord*);
void __fastcall UpdateMonoAudioSource(AudioBackendStateView*, void*, AudioSourceRecord*);
void __fastcall UpdateStereoAudioSource(AudioBackendStateView*, void*, AudioSourceRecord*);
void __fastcall UpdateThreeDimensionalAudioSource(AudioBackendStateView*, void*, AudioSourceRecord*);

static AudioBackendStateView* __fastcall DeleteMonoProvider(AudioBackendStateView* p_provider, void*, LegoU32 p_flags)
{
	MonoDirectSoundProvider* provider = (MonoDirectSoundProvider*) p_provider;
	provider->m_vtable = &g_monoDirectSoundProviderDispatch;
	provider->ReleaseDeviceInterfaces();
	provider->m_vtable = &g_audioBackendBaseDispatch;
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_provider);
	}
	return p_provider;
}

static AudioBackendStateView* __fastcall DeleteStereoProvider(AudioBackendStateView* p_provider, void*, LegoU32 p_flags)
{
	StereoDirectSoundProvider* provider = (StereoDirectSoundProvider*) p_provider;
	provider->m_vtable = &g_stereoDirectSoundProviderDispatch;
	provider->ReleaseDeviceInterfaces();
	provider->m_vtable = &g_audioBackendBaseDispatch;
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_provider);
	}
	return p_provider;
}

static AudioBackendStateView* __fastcall DeleteThreeDimensionalProvider(
	AudioBackendStateView* p_provider,
	void*,
	LegoU32 p_flags
)
{
	ThreeDimensionalDirectSoundProvider* provider = (ThreeDimensionalDirectSoundProvider*) p_provider;
	provider->m_vtable = &g_threeDimensionalDirectSoundProviderDispatch;
	provider->ReleaseDeviceInterfaces();
	provider->m_vtable = &g_audioBackendBaseDispatch;
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_provider);
	}
	return p_provider;
}

static void __fastcall InitializeMonoProvider(AudioBackendStateView* p_provider, void*, HWND p_window)
{
	((MonoDirectSoundProvider*) p_provider)->Initialize(p_window);
}

static void __fastcall InitializeStereoProvider(AudioBackendStateView* p_provider, void*, HWND p_window)
{
	((StereoDirectSoundProvider*) p_provider)->Initialize(p_window);
}

static void __fastcall InitializeThreeDimensionalProvider(AudioBackendStateView* p_provider, void*, HWND p_window)
{
	((ThreeDimensionalDirectSoundProvider*) p_provider)->Initialize(p_window);
}

static void __fastcall ReleaseMonoProviderInterfaces(AudioBackendStateView* p_provider, void*)
{
	((MonoDirectSoundProvider*) p_provider)->ReleaseDeviceInterfaces();
}

static void __fastcall ReleaseStereoProviderInterfaces(AudioBackendStateView* p_provider, void*)
{
	((StereoDirectSoundProvider*) p_provider)->ReleaseDeviceInterfaces();
}

static void __fastcall ReleaseThreeDimensionalProviderInterfaces(AudioBackendStateView* p_provider, void*)
{
	((ThreeDimensionalDirectSoundProvider*) p_provider)->ReleaseDeviceInterfaces();
}

static void __fastcall ReleaseMonoBufferPointer(AudioBackendStateView* p_provider, void*, IDirectSoundBuffer** p_buffer)
{
	((MonoDirectSoundProvider*) p_provider)->ReleasePlaybackObject(p_buffer);
}

static void __fastcall ReleaseStereoBufferPointer(
	AudioBackendStateView* p_provider,
	void*,
	IDirectSoundBuffer** p_buffer
)
{
	if (*p_buffer != NULL) {
		StopAudioPlaybackObject(*p_buffer);
		(*p_buffer)->Release();
		*p_buffer = NULL;
	}
}

static void __fastcall ReleaseThreeDimensionalBufferPointer(
	AudioBackendStateView* p_provider,
	void*,
	IDirectSoundBuffer** p_buffer
)
{
	if (*p_buffer != NULL) {
		StopAudioPlaybackObject(*p_buffer);
		(*p_buffer)->Release();
		*p_buffer = NULL;
	}
}

static IDirectSoundBuffer* __fastcall DuplicateMonoPlaybackBuffer(
	AudioBackendStateView* p_provider,
	void*,
	IDirectSoundBuffer* p_source
)
{
	IDirectSoundBuffer* result = p_source;
	if (((MonoDirectSoundProvider*) p_provider)->m_device->DuplicateSoundBuffer(p_source, &result) != DS_OK) {
		return NULL;
	}
	((MonoDirectSoundProvider*) p_provider)->m_device->Compact();
	return result;
}

static IDirectSoundBuffer* __fastcall DuplicateStereoPlaybackBuffer(
	AudioBackendStateView* p_provider,
	void*,
	IDirectSoundBuffer* p_source
)
{
	IDirectSoundBuffer* result = p_source;
	if (((StereoDirectSoundProvider*) p_provider)->m_device->DuplicateSoundBuffer(p_source, &result) != DS_OK) {
		return NULL;
	}
	((StereoDirectSoundProvider*) p_provider)->m_device->Compact();
	return result;
}

static IDirectSoundBuffer* __fastcall DuplicateThreeDimensionalPlaybackBuffer(
	AudioBackendStateView* p_provider,
	void*,
	IDirectSoundBuffer* p_source
)
{
	IDirectSoundBuffer* result = p_source;
	if (((ThreeDimensionalDirectSoundProvider*) p_provider)->m_device->DuplicateSoundBuffer(p_source, &result) !=
		DS_OK) {
		return NULL;
	}
	((ThreeDimensionalDirectSoundProvider*) p_provider)->m_device->Compact();
	return result;
}

static void __fastcall ReleaseMonoSource(AudioBackendStateView* p_provider, void*, AudioSourceRecord* p_source)
{
	((MonoDirectSoundProvider*) p_provider)->ReleaseSourcePlaybackBuffer(p_source);
}

static void __fastcall ProviderNoOp(AudioBackendStateView*, void*)
{
}

static LegoBool32 __fastcall StartProviderPlayback(
	AudioBackendStateView*,
	void*,
	IDirectSoundBuffer* p_buffer,
	LegoU32 p_loop
)
{
	return StartAudioPlaybackObject(p_buffer, p_loop);
}

static LegoBool32 __fastcall StopProviderPlayback(AudioBackendStateView*, void*, IDirectSoundBuffer* p_buffer)
{
	return StopAudioPlaybackObject(p_buffer);
}

static LegoBool32 __fastcall IsProviderPlaybackActive(AudioBackendStateView*, void*, IDirectSoundBuffer* p_buffer)
{
	return IsAudioPlaybackObjectActive(p_buffer);
}

static LegoBool32 __fastcall SetProviderPlaybackFrequency(
	AudioBackendStateView*,
	void*,
	IDirectSoundBuffer* p_buffer,
	LegoS32 p_frequency
)
{
	return SetAudioPlaybackFrequency(p_buffer, p_frequency);
}

static LegoBool32 __fastcall ResetProviderPlaybackPosition(AudioBackendStateView*, void*, IDirectSoundBuffer* p_buffer)
{
	return ResetAudioPlaybackPosition(p_buffer);
}

// STUB: LEGORACERS2 0x004ef9b0
static AudioBackendStateView* __fastcall DeleteBaseAudioProvider(
	AudioBackendStateView* p_provider,
	void*,
	LegoU32 p_flags
)
{
	p_provider->m_vtable = &g_audioBackendBaseDispatch;
	if ((p_flags & 1) != 0) {
		FreeHeapMemory(p_provider);
	}
	return p_provider;
}

// Retail table 0x00536f8c; every slot is checked by tools/check_audio_dispatch.py.
// ABI bridges and original method bodies are measured separately.
const AudioBackendStateDispatch g_audioBackendBaseDispatch = {
	DeleteBaseAudioProvider,
	(AudioBackendStateDispatch::Initialize) _purecall,
	(AudioBackendStateDispatch::Action) _purecall,
	(AudioBackendStateDispatch::CreateSourceBuffer) _purecall,
	(AudioBackendStateDispatch::ReleaseBufferPointer) _purecall,
	(AudioBackendStateDispatch::DuplicatePlaybackBuffer) _purecall,
	(AudioBackendStateDispatch::ReleaseSourceBuffer) _purecall,
	(AudioBackendStateDispatch::Action) _purecall,
	(AudioBackendStateDispatch::GetSourceSampleRate) _purecall,
	(AudioBackendStateDispatch::StartPlayback) _purecall,
	(AudioBackendStateDispatch::StopPlayback) _purecall,
	(AudioBackendStateDispatch::IsPlaybackActive) _purecall,
	(AudioBackendStateDispatch::SetPlaybackFrequency) _purecall,
	(AudioBackendStateDispatch::PlaybackAction) _purecall,
	(AudioBackendStateDispatch::UpdateSource) _purecall,
};

// Retail table 0x00536f50; every slot is checked by tools/check_audio_dispatch.py.
// ABI bridges and original method bodies are measured separately.
const AudioBackendStateDispatch g_threeDimensionalDirectSoundProviderDispatch = {
	DeleteThreeDimensionalProvider,
	InitializeThreeDimensionalProvider,
	ReleaseThreeDimensionalProviderInterfaces,
	CreateThreeDimensionalSourceBuffer,
	ReleaseThreeDimensionalBufferPointer,
	DuplicateThreeDimensionalPlaybackBuffer,
	ReleaseThreeDimensionalSourceBuffer,
	ProviderNoOp,
	GetWaveSampleRate,
	StartProviderPlayback,
	StopProviderPlayback,
	IsProviderPlaybackActive,
	SetProviderPlaybackFrequency,
	ResetProviderPlaybackPosition,
	UpdateThreeDimensionalAudioSource,
};

// Retail table 0x00536fcc; every slot is checked by tools/check_audio_dispatch.py.
// ABI bridges and original method bodies are measured separately.
const AudioBackendStateDispatch g_stereoDirectSoundProviderDispatch = {
	DeleteStereoProvider,
	InitializeStereoProvider,
	ReleaseStereoProviderInterfaces,
	CreatePcmSourceBuffer,
	ReleaseStereoBufferPointer,
	DuplicateStereoPlaybackBuffer,
	ReleaseStereoSourceBuffer,
	ProviderNoOp,
	GetWaveSampleRate,
	StartProviderPlayback,
	StopProviderPlayback,
	IsProviderPlaybackActive,
	SetProviderPlaybackFrequency,
	ResetProviderPlaybackPosition,
	UpdateStereoAudioSource,
};

// Retail table 0x0053700c; every slot is checked by tools/check_audio_dispatch.py.
// ABI bridges and original method bodies are measured separately.
const AudioBackendStateDispatch g_monoDirectSoundProviderDispatch = {
	DeleteMonoProvider,
	InitializeMonoProvider,
	ReleaseMonoProviderInterfaces,
	CreatePcmSourceBuffer,
	ReleaseMonoBufferPointer,
	DuplicateMonoPlaybackBuffer,
	ReleaseMonoSource,
	ProviderNoOp,
	GetWaveSampleRate,
	StartProviderPlayback,
	StopProviderPlayback,
	IsProviderPlaybackActive,
	SetProviderPlaybackFrequency,
	ResetProviderPlaybackPosition,
	UpdateMonoAudioSource,
};
