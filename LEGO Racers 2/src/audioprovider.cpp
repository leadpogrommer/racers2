#include "audioprovider.h"

#include "audioformat.h"
#include "audioservice.h"

struct DirectSoundOutputProviderPrefix {
	const void* m_vtable;
	IDirectSound8* m_device;
	IDirectSoundBuffer* m_primaryBuffer;
	LegoU32 m_initializationStatus;
};

DECOMP_STATIC_ASSERT(offsetof(DirectSoundOutputProviderPrefix, m_device) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(DirectSoundOutputProviderPrefix, m_initializationStatus) == 0x0c);

// This is a source-level helper for the shared setup sequence used by the
// stereo and mono DirectSound providers; it does not claim either retail ABI.
LegoBool32 InitializeDirectSoundOutput(
	IDirectSound8** p_device,
	IDirectSoundBuffer** p_primaryBuffer,
	LegoU32* p_initializationStatus,
	HWND p_window,
	LegoBool32 p_mono
)
{
	HRESULT result = DirectSoundCreate8(NULL, p_device, NULL);
	if (result != DS_OK) {
		if (p_mono == FALSE) {
			*p_device = NULL;
		}
		return FALSE;
	}

	IDirectSound8* device = *p_device;
	result = device->SetCooperativeLevel(p_window, DSSCL_PRIORITY);
	if (result != DS_OK) {
		device->Release();
		return FALSE;
	}

	DSBUFFERDESC description;
	ZeroMemory(&description, sizeof(description));
	description.dwSize = 0x24;
	description.dwFlags = DSBCAPS_PRIMARYBUFFER;

	IDirectSoundBuffer* primaryBuffer;
	result = device->CreateSoundBuffer(&description, p_primaryBuffer, NULL);
	if (result != DS_OK) {
		device->Release();
		return FALSE;
	}

	WAVEFORMATEX format;
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = p_mono ? 1 : 2;
	format.nSamplesPerSec = 0x5654;
	format.nAvgBytesPerSec = p_mono ? 0xaca8 : 0x15950;
	format.nBlockAlign = p_mono ? 2 : 4;
	format.wBitsPerSample = 16;
	format.cbSize = 0;

	primaryBuffer = *p_primaryBuffer;
	result = primaryBuffer->SetFormat(&format);
	if (result != DS_OK) {
		primaryBuffer->Release();
		device->Release();
		return FALSE;
	}

	if (primaryBuffer->Release() != 0) {
		device->Release();
		return FALSE;
	}
	*p_primaryBuffer = NULL;
	*p_initializationStatus = TRUE;
	return TRUE;
}

// STUB: LEGORACERS2 0x004efdd0
LegoU32 __fastcall GetWaveSampleRate(AudioBackendStateView*, void*, const void* p_source, LegoU32)
{
	WAVEFORMATEX* format = NULL;
	const LegoU8* sampleData = NULL;
	LegoU32 sampleByteCount = 0;
	if (ParseWaveOrAiffAudioData(p_source, &format, &sampleData, &sampleByteCount, NULL) && format != NULL &&
		sampleData != NULL && sampleByteCount != 0) {
		return format->nSamplesPerSec;
	}
	return 0;
}

// STUB: LEGORACERS2 0x004efe40
LegoBool32 __stdcall StopAudioPlaybackObject(AudioPlaybackObjectView* p_object)
{
	if (p_object == NULL) {
		return FALSE;
	}

	if (IsAudioPlaybackObjectActive(p_object) != FALSE) {
		if (p_object->Stop() != DS_OK) {
			return FALSE;
		}
	}

	return TRUE;
}

// STUB: LEGORACERS2 0x004f0690
void __fastcall ReleaseDirectSoundOutputInterfaces(DirectSoundOutputProviderPrefix* p_provider, void*)
{
	if (p_provider->m_primaryBuffer != NULL) {
		p_provider->m_primaryBuffer->Release();
		p_provider->m_primaryBuffer = NULL;
	}

	if (p_provider->m_device != NULL) {
		p_provider->m_device->Release();
		p_provider->m_device = NULL;
	}
}

void MonoDirectSoundProvider::ReleaseDeviceInterfaces()
{
	ReleaseDirectSoundOutputInterfaces((DirectSoundOutputProviderPrefix*) this, NULL);
}

// STUB: LEGORACERS2 0x004f0aa0
MonoDirectSoundProvider::MonoDirectSoundProvider(HWND p_window)
{
	m_vtable = &g_monoDirectSoundProviderDispatch;
	m_device = NULL;
	m_primaryBuffer = NULL;
	m_initializationStatus = 0;
	Initialize(p_window);
}

// STUB: LEGORACERS2 0x004f0b10
void MonoDirectSoundProvider::Initialize(HWND p_window)
{
	InitializeDirectSoundOutput(&m_device, &m_primaryBuffer, &m_initializationStatus, p_window, TRUE);
}

// STUB: LEGORACERS2 0x004f0e30
void MonoDirectSoundProvider::ReleasePlaybackObject(AudioPlaybackObjectView** p_object)
{
	if (*p_object != NULL) {
		StopAudioPlaybackObject(*p_object);
		(*p_object)->Release();
		*p_object = NULL;
	}
}

// STUB: LEGORACERS2 0x004f0ea0
void MonoDirectSoundProvider::ReleaseSourcePlaybackBuffer(AudioSourceRecord* p_source)
{
	if (p_source->m_playbackBuffer != NULL) {
		AudioBackendStateView* provider = (AudioBackendStateView*) this;
		provider->m_vtable->m_stopPlayback(provider, NULL, p_source->m_playbackBuffer);
		p_source->m_playbackBuffer->Release();
		p_source->m_playbackBuffer = NULL;
	}
}

// STUB: LEGORACERS2 0x004f0ed0
LegoBool32 __stdcall StartAudioPlaybackObject(AudioPlaybackObjectView* p_object, LegoU32 p_loop)
{
	if (p_object == NULL) {
		return FALSE;
	}

	if (IsAudioPlaybackObjectActive(p_object) == FALSE &&
		p_object->Play(0, 0, p_loop != 0 ? DSBPLAY_LOOPING : 0) != DS_OK) {
		return FALSE;
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004f0f20
LegoBool32 __stdcall IsAudioPlaybackObjectActive(AudioPlaybackObjectView* p_object)
{
	if (p_object == NULL) {
		return FALSE;
	}

	DWORD state;
	if (p_object->GetStatus(&state) != DS_OK) {
		return FALSE;
	}

	return (LegoBool32) (state & 1);
}

// STUB: LEGORACERS2 0x004f0f50
LegoBool32 __stdcall SetAudioPlaybackFrequency(AudioPlaybackObjectView* p_object, LegoS32 p_frequency)
{
	if (p_object == NULL) {
		return FALSE;
	}

	LegoS32 frequency = p_frequency;
	if (frequency < 100) {
		frequency = 100;
	}
	else if (frequency > 100000) {
		frequency = 100000;
	}

	DWORD currentFrequency;
	if (p_object->GetFrequency(&currentFrequency) != DS_OK || currentFrequency != (LegoU32) frequency) {
		if (p_object->SetFrequency((LegoU32) frequency) != DS_OK) {
			return FALSE;
		}
	}

	return TRUE;
}

// STUB: LEGORACERS2 0x004f0fb0
LegoBool32 __stdcall ResetAudioPlaybackPosition(AudioPlaybackObjectView* p_object)
{
	if (p_object == NULL) {
		return FALSE;
	}

	return p_object->SetCurrentPosition(0) == DS_OK;
}
