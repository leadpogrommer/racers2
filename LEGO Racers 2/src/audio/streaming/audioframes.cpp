#include "audio/streaming/audioframes.h"

#include <stdlib.h>
#include <string.h>

namespace
{
struct SourceFadeState {
	LegoFloat m_activeWaitMin;
	LegoFloat m_activeWaitMax;
	LegoFloat m_inactiveWaitMin;
	LegoFloat m_inactiveWaitMax;
	LegoFloat m_fadeInTime;
	LegoFloat m_fadeOutTime;
	LegoFloat m_duration;
	LegoFloat m_elapsed;
	LegoU32 m_phase;
	LegoFloat m_gain;
};
DECOMP_SIZE_ASSERT(SourceFadeState, 0x28);

LegoFloat ChooseAudioWait(LegoFloat p_min, LegoFloat p_max)
{
	LegoFloat range = p_max - p_min;
	if (range == 0.0f) {
		return p_min;
	}
	LegoS32 roundedRange;
	// The original random interval observes the current x87 rounding mode.
	__asm {
		fld range
		fistp roundedRange
	}
	LegoFloat remainder = (LegoFloat) (rand() % roundedRange);
	return (LegoFloat) (LegoS32) remainder + p_min + 1.0f;
}

void SetAudioWait(SourceFadeState& p_fade, LegoFloat p_min, LegoFloat p_max)
{
	p_fade.m_phase = 0;
	p_fade.m_elapsed = 0.0f;
	p_fade.m_duration = ChooseAudioWait(p_min, p_max);
}

void UpdateSourceFade(AudioSourceRecord* p_source, LegoFloat p_elapsed)
{
	SourceFadeState& fade = *(SourceFadeState*) p_source->m_unk0x90;
	if ((p_source->m_unk0xb8 & 0x200) == 0 || fade.m_inactiveWaitMax == 0.0f) {
		fade.m_gain = 1.0f;
		return;
	}
	if (!(fade.m_inactiveWaitMax - fade.m_inactiveWaitMin >= 0.0f) ||
		!(fade.m_activeWaitMax - fade.m_activeWaitMin >= 0.0f)) {
		return;
	}
	fade.m_elapsed += p_elapsed;
	switch (fade.m_phase) {
	case 0:
		if (fade.m_duration <= fade.m_elapsed) {
			if (p_source->m_unk0x30 == 1 || p_source->m_unk0x34 == 1) {
				if (fade.m_fadeInTime == 0.0f) {
					SetAudioWait(fade, fade.m_inactiveWaitMin, fade.m_inactiveWaitMax);
					p_source->m_vtable->m_resetPlaybackState(p_source, NULL);
				}
				else {
					p_source->m_vtable->m_resetPlaybackState(p_source, NULL);
					fade.m_phase = 2;
					fade.m_elapsed = 0;
					fade.m_duration = fade.m_fadeInTime;
				}
			}
			else if (fade.m_fadeOutTime == 0.0f) {
				SetAudioWait(fade, fade.m_activeWaitMin, fade.m_activeWaitMax);
				p_source->m_vtable->m_clearSourceFlag(p_source, NULL);
			}
			else {
				fade.m_phase = 1;
				fade.m_elapsed = 0;
				fade.m_duration = fade.m_fadeOutTime;
			}
		}
		break;
	case 1:
		if (fade.m_gain > 0.0f) {
			fade.m_gain = 1.0f - fade.m_elapsed / fade.m_duration;
		}
		else {
			p_source->m_vtable->m_clearSourceFlag(p_source, NULL);
			SetAudioWait(fade, fade.m_inactiveWaitMin, fade.m_inactiveWaitMax);
		}
		break;
	case 2:
		if (fade.m_gain < 1.0f) {
			fade.m_gain = fade.m_elapsed / fade.m_duration;
		}
		else {
			p_source->m_vtable->m_resetPlaybackState(p_source, NULL);
			SetAudioWait(fade, fade.m_activeWaitMin, fade.m_activeWaitMax);
		}
		break;
	}
}

LegoFloat GetSourcePriority(AudioSourceRecord* p_source)
{
	return (10.0f - (LegoFloat) (p_source->m_unk0xb8 & 0xf)) * 50.0f + *(LegoFloat*) &p_source->m_unk0xd4;
}
} // namespace

// STUB: LEGORACERS2 0x004af980
void __fastcall ProcessAudioSourceFrames(AudioService* p_service, void*)
{
	p_service->m_unk0x20 = 0;
	for (LegoU32 i = 0; i < 512; ++i) {
		AudioSourceRecord* source = &p_service->m_sources[i];
		source->m_unk0xd0 = 0;
		source->m_unk0x38 = 0;
		UpdateSourceFade(source, p_service->m_elapsedTime);
		g_audioBackend->m_vtable->m_updateSource(g_audioBackend, NULL, source);
		LegoU32 flags = source->m_unk0xb8;
		if (source->m_unk0x2c == 0 || source->m_unk0x34 != 0 || (flags & 0x10) == 0 ||
			(p_service->m_musicEnabled != 0 && (flags & 0x4000) == 0)) {
			continue;
		}
		if (source->m_unk0x40 != 0) {
			source->m_playbackBuffer->SetVolume(-8000);
			if (source->m_unk0x44 == 0) {
				CharBuffer24 name;
				memcpy(name.m_data, source->m_unkName0x4c, sizeof(name.m_data));
				source->m_unk0x44 = (LegoU32) FindAudioGameObject(name);
			}
			source->m_unk0x40 = 0;
		}
		if ((flags & 0x400) == 0 && source->m_unk0x30 == 0 &&
			g_audioBackend->m_vtable->m_isPlaybackActive(g_audioBackend, NULL, source->m_playbackBuffer) == FALSE) {
			continue;
		}
		if (flags & 0x80) {
			Vector3 position;
			if (source->m_unk0x48 == 0) {
				position = *(Vector3*) &source->m_unk0x04;
			}
			else if (HasAudioGameObjectPosition((GameObject*) source->m_unk0x44)) {
				GetAudioGameObjectPosition((GameObject*) source->m_unk0x44, &position);
			}
			else {
				position = *(Vector3*) &source->m_unk0x10;
			}
			Vector3 listener;
			FindNearestAudioListener(p_service, NULL, &position, &listener);
			position.m_x -= listener.m_x;
			position.m_y -= listener.m_y;
			position.m_z -= listener.m_z;
			LegoFloat distance = GetAudioVectorLength(&position, NULL);
			*(LegoFloat*) &source->m_unk0xd4 = distance;
			if (source->m_defaultRange + 50.0f < distance) {
				continue;
			}
		}
		AudioSourceRecord** link = (AudioSourceRecord**) &p_service->m_unk0x20;
		for (LegoS32 rank = 0; rank < (LegoS32) p_service->m_unk0x1c; ++rank) {
			AudioSourceRecord* current = *link;
			if (current == NULL || GetSourcePriority(source) < GetSourcePriority(current) || (flags & 0x80) == 0) {
				source->m_unk0xd0 = (LegoU32) current;
				*link = source;
				break;
			}
			link = (AudioSourceRecord**) &current->m_unk0xd0;
		}
	}
	AudioSourceRecord* source = (AudioSourceRecord*) p_service->m_unk0x20;
	for (LegoS32 rank = 0; rank < (LegoS32) p_service->m_unk0x1c; ++rank) {
		if (source == NULL) {
			return;
		}
		source->m_unk0x38 = 1;
		source = (AudioSourceRecord*) source->m_unk0xd0;
	}
}
