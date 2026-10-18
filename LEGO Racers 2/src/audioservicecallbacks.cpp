#include "audiocommentary.h"
#include "audiomusiccontroller.h"
#include "audioprovider.h"
#include "audioservice.h"

LegoFloat __fastcall GetAudioFrameDuration(AudioService* p_service, void* p_edx);
void __fastcall ProcessAudioSourceFrames(AudioService* p_service, void* p_edx);
void __fastcall UpdateAudioDeviceBuffers(AudioService* p_service, void* p_edx);
void __fastcall UpdateAudioSourceOutputs(AudioService* p_service, void* p_edx);
// STUB: LEGORACERS2 0x004af7e0
void AudioService::SetMusicEnabled(LegoU32 p_enabled)
{
	LegoU32 enabled = p_enabled != 0;
	m_musicEnabled = enabled;
	if (m_unk0x249f4 != NULL) {
		SetDigitalMusicControllerEnabled((DigitalMusicController*) m_unk0x249f4, NULL, p_enabled);
	}
}

// STUB: LEGORACERS2 0x004af810
void AudioService::Update(LegoU32)
{
	if (m_initializationFailed != 0 || m_shutdown != 0) {
		return;
	}

	m_elapsedTime = GetAudioFrameDuration(this, NULL);
	ProcessAudioSourceFrames(this, NULL);
	UpdateAudioDeviceBuffers(this, NULL);
	UpdateAudioSourceOutputs(this, NULL);
	g_audioBackend->m_vtable->m_unk0x1c(g_audioBackend, NULL);
	if (m_unk0x24ba0 != 0) {
		PlayNextAudioCommentary(this, NULL);
	}
}

// STUB: LEGORACERS2 0x004af860
void __fastcall UpdateAudioDeviceBuffers(AudioService* p_service, void*)
{
	for (LegoU32 i = 0; i < 512; ++i) {
		AudioSourceRecord* source = &p_service->m_sources[i];
		if (source->m_unk0x2c != 0 && source->m_unk0x38 == 0 && (source->m_unk0x34 == 0 || source->m_unk0x30 == 0)) {
			LegoU32 flags = source->m_unk0xb8;
			g_audioBackend->m_vtable->m_stopPlayback(g_audioBackend, NULL, source->m_playbackBuffer);
			if (((flags >> 10) & 1) != 0) {
				source->m_unk0x30 = 1;
			}
			else {
				source->m_unk0x34 = 1;
			}
		}
	}
}

// Retail stores the scaled value to a float local before FISTP, which observes
// the active x87 rounding mode.
static LegoS32 ConvertAudioScaleToInteger(LegoFloat p_value)
{
	volatile LegoFloat storedValue = p_value;
	LegoS32 result;
	__asm {
		fld storedValue
		fistp result
	}
	return result;
}

// STUB: LEGORACERS2 0x004af8d0
void __fastcall UpdateAudioSourceOutputs(AudioService* p_service, void*)
{
	AudioSourceRecord* source = (AudioSourceRecord*) p_service->m_unk0x20;
	while (source != NULL) {
		LegoU32 flags = source->m_unk0xb8;
		IDirectSoundBuffer* playback = source->m_playbackBuffer;
		if (source->m_unk0x3c != 0) {
			if ((flags & 0x400) == 0) {
				g_audioBackend->m_vtable->m_resetPlaybackPosition(g_audioBackend, NULL, playback);
			}
			source->m_unk0x3c = 0;
		}
		if (source->m_unk0xcc != 0) {
			LegoFloat scaled = (LegoFloat) source->m_unk0xcc * source->m_unk0x7c;
			g_audioBackend->m_vtable
				->m_setPlaybackFrequency(g_audioBackend, NULL, playback, ConvertAudioScaleToInteger(scaled));
		}
		if (source->m_unk0x30 != 0) {
			g_audioBackend->m_vtable->m_startPlayback(g_audioBackend, NULL, playback, (flags >> 10) & 1);
			source->m_unk0x30 = 0;
		}
		source = (AudioSourceRecord*) source->m_unk0xd0;
	}
}

// STUB: LEGORACERS2 0x004b13d0
LegoU32 __fastcall GetAudioServiceShutdownStatus(AudioService* p_service, void*)
{
	return p_service->m_shutdown;
}
