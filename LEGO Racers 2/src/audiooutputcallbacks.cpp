#include "audioformat.h"
#include "audioprovider.h"
#include "audioservice.h"

#include <string.h>

static IDirectSoundBuffer* CreateOutputBuffer(
	AudioBackendStateView* p_provider,
	const void* p_audioFile,
	DWORD p_flags,
	const GUID* p_algorithm
)
{
	WAVEFORMATEX* format = NULL;
	const LegoU8* sampleData = NULL;
	LegoU32 sampleByteCount = 0;
	LegoU32 bitsPerSample = 0;
	if (!ParseWaveOrAiffAudioData(p_audioFile, &format, &sampleData, &sampleByteCount, &bitsPerSample) ||
		format == NULL || sampleData == NULL || sampleByteCount == 0) {
		return NULL;
	}

	DSBUFFERDESC description;
	ZeroMemory(&description, sizeof(description));
	description.dwSize = 0x24;
	description.dwFlags = p_flags;
	description.dwBufferBytes = sampleByteCount;
	description.lpwfxFormat = format;
	if (p_algorithm != NULL) {
		description.guid3DAlgorithm = *p_algorithm;
	}

	IDirectSoundBuffer* buffer = NULL;
	if (p_provider->m_device->CreateSoundBuffer(&description, &buffer, NULL) != DS_OK) {
		return NULL;
	}

	void* firstRegion = NULL;
	void* secondRegion = NULL;
	DWORD firstBytes = 0;
	DWORD secondBytes = 0;
	if (buffer->Lock(0, sampleByteCount, &firstRegion, &firstBytes, &secondRegion, &secondBytes, 2) != DS_OK) {
		buffer->Release();
		return NULL;
	}

	DWORD firstCopyBytes = firstBytes;
	if (firstCopyBytes > sampleByteCount) {
		firstCopyBytes = sampleByteCount;
	}
	if (firstCopyBytes != 0) {
		if (format == &g_aiffPcmFormat) {
			ConvertBigEndianPcm((LegoU8*) firstRegion, sampleData, firstCopyBytes, bitsPerSample);
		}
		else {
			memcpy(firstRegion, sampleData, firstCopyBytes);
		}
	}

	DWORD secondCopyBytes = secondBytes;
	if (secondCopyBytes > sampleByteCount - firstCopyBytes) {
		secondCopyBytes = sampleByteCount - firstCopyBytes;
	}
	if (secondCopyBytes != 0) {
		if (format == &g_aiffPcmFormat) {
			ConvertBigEndianPcm((LegoU8*) secondRegion, sampleData + firstCopyBytes, secondCopyBytes, bitsPerSample);
		}
		else {
			memcpy(secondRegion, sampleData + firstCopyBytes, secondCopyBytes);
		}
	}

	if (buffer->Unlock(firstRegion, firstBytes, secondRegion, secondBytes) != DS_OK) {
		buffer->Release();
		return NULL;
	}
	return buffer;
}

// STUB: LEGORACERS2 0x004efb20
IDirectSoundBuffer* __fastcall CreateThreeDimensionalSourceBuffer(
	AudioBackendStateView* p_provider,
	void*,
	const void* p_audioFile,
	LegoU32
)
{
	return CreateOutputBuffer(p_provider, p_audioFile, 0x000400b0, &DS3DALG_HRTF_FULL);
}

// STUB: LEGORACERS2 0x004efd60
void __fastcall ReleaseThreeDimensionalSourceBuffer(
	AudioBackendStateView* p_provider,
	void*,
	AudioSourceRecord* p_source
)
{
	if (p_source->m_playbackBuffer != NULL) {
		p_provider->m_vtable->m_stopPlayback(p_provider, NULL, p_source->m_playbackBuffer);
		if (p_source->m_unk0xc8 != 0) {
			((IUnknown*) p_source->m_unk0xc8)->Release();
			p_source->m_unk0xc8 = 0;
		}
		if (p_source->m_unk0xc4 != 0) {
			((IUnknown*) p_source->m_unk0xc4)->Release();
			p_source->m_unk0xc4 = 0;
		}
		p_source->m_playbackBuffer->Release();
		p_source->m_playbackBuffer = NULL;
	}
}

static LegoBool32 __fastcall CanStopStereoAudioBuffer(AudioBackendStateView*, void*, IDirectSoundBuffer*);

// STUB: LEGORACERS2 0x004f06c0
void __fastcall ReleaseStereoSourceBuffer(AudioBackendStateView* p_provider, void*, AudioSourceRecord* p_source)
{
	if (p_source->m_playbackBuffer != NULL) {
		CanStopStereoAudioBuffer(p_provider, NULL, p_source->m_playbackBuffer);
		p_source->m_playbackBuffer->Release();
		p_source->m_playbackBuffer = NULL;
	}
}

// STUB: LEGORACERS2 0x004f06f0
static LegoBool32 __fastcall CanStopStereoAudioBuffer(
	AudioBackendStateView* p_provider,
	void*,
	IDirectSoundBuffer* p_buffer
)
{
	if (p_buffer == NULL) {
		return FALSE;
	}
	if (p_provider->m_vtable->m_isPlaybackActive(p_provider, NULL, p_buffer) != FALSE && p_buffer->Stop() != DS_OK) {
		return FALSE;
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004f0c50
IDirectSoundBuffer* __fastcall CreatePcmSourceBuffer(
	AudioBackendStateView* p_provider,
	void*,
	const void* p_audioFile,
	LegoU32
)
{
	return CreateOutputBuffer(
		p_provider,
		p_audioFile,
		DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME,
		NULL
	);
}
