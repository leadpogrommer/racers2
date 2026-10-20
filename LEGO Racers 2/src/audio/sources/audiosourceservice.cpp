#include "audio/output/audioprovider.h"
#include "audio/service/audioservice.h"
#include "audio/sources/audiochannelreload.h"
#include "core/debug/diagnostics.h"

#include <string.h>

struct AudioSourceDefinition {
	LegoChar m_name[24];          // 0x00
	LegoChar m_secondaryName[24]; // 0x18
	LegoChar m_audioPath[128];    // 0x30
	LegoU8 m_unk0xb0[0x18];
	LegoFloat m_gainA;       // 0xc8
	LegoFloat m_gainB;       // 0xcc
	LegoFloat m_maximumGain; // 0xd0
	LegoU32 m_musicFlag;     // 0xd4
	LegoU32 m_spatialFlag;   // 0xd8
	LegoU32 m_distanceFlag;  // 0xdc
	LegoU32 m_lowFlags;      // 0xe0
	LegoFloat m_falloff;     // 0xe4
	LegoFloat m_range;       // 0xe8
	LegoU32 m_unk0xec;
	LegoU32 m_unk0xf0;
	LegoU32 m_unk0xf4;
	LegoU32 m_unk0xf8;
	LegoU32 m_audioValue0; // 0xfc
	LegoU32 m_audioValue1;
	LegoU32 m_audioValue2;
	LegoU32 m_audioValue3;
	LegoU32 m_audioValue4;
	LegoU32 m_audioValue5;    // 0x110
	LegoU32 m_disableFalloff; // 0x114
};

DECOMP_STATIC_ASSERT(offsetof(AudioSourceDefinition, m_audioPath) == 0x30);
DECOMP_STATIC_ASSERT(offsetof(AudioSourceDefinition, m_gainA) == 0xc8);
DECOMP_STATIC_ASSERT(offsetof(AudioSourceDefinition, m_disableFalloff) == 0x114);

AudioSourceRecord* __fastcall FindAvailableAudioSourceRecord(AudioService* p_service, void* p_edx);

static LegoU32 IsSameSourceName(const LegoChar* p_first, const LegoChar* p_second)
{
	for (LegoU32 i = 0; i < 24; ++i) {
		if (p_first[i] != p_second[i]) {
			return FALSE;
		}
		if (p_first[i] == '\0') {
			return TRUE;
		}
	}
	return TRUE;
}

// The channel cache is implemented in the adjacent source-file helper unit.
extern void __fastcall ReleaseAudioChannelRecordBuffer(AudioChannelRecord* p_channel, void* p_edx);
extern AudioChannelRecord* __fastcall FindOrCreateAudioChannel(
	AudioService* p_service,
	void* p_edx,
	const LegoChar* p_path
);

// STUB: LEGORACERS2 0x004aff40
void __fastcall AddAudioSource(AudioService* p_service, void*, const LegoU32* p_parameters, LegoU32 p_sourceId, LegoU32)
{
	const AudioSourceDefinition* definition = (const AudioSourceDefinition*) p_parameters;
	AudioSourceRecord* source = FindAvailableAudioSourceRecord(p_service, NULL);
	if (source == NULL) {
		return;
	}

	source->m_unk0x44 = p_sourceId;
	memcpy(source->m_unkName0x4c, definition->m_name, sizeof(source->m_unkName0x4c));
	memcpy(source->m_unkName0x64, definition->m_secondaryName, sizeof(source->m_unkName0x64));
	static const LegoChar kNonCObject[] = "NON C OBJECT";
	source->m_unk0x48 = IsSameSourceName(source->m_unkName0x4c, kNonCObject) == TRUE ? FALSE : TRUE;

	if (!(definition->m_gainA >= 0.0f)) {
		source->m_unk0x80 = 0.0f;
	}
	else if (definition->m_gainA <= 1.0f) {
		source->m_unk0x80 = definition->m_gainA;
	}
	else {
		source->m_unk0x80 = 1.0f;
	}

	if (!(definition->m_gainB >= 0.0f)) {
		source->m_unk0x84 = 0.0f;
	}
	else if (definition->m_gainB <= 1.0f) {
		source->m_unk0x84 = definition->m_gainB;
	}
	else {
		source->m_unk0x84 = 1.0f;
	}

	if (!(definition->m_maximumGain >= 0.0f)) {
		source->m_unk0x7c = 0.0f;
	}
	else if (definition->m_maximumGain <= 4.0f) {
		source->m_unk0x7c = definition->m_maximumGain;
	}
	else {
		source->m_unk0x7c = 4.0f;
	}

	source->m_unk0xb8 = (definition->m_disableFalloff != 0 ? 0x4000 : 0) | (definition->m_unk0xf0 != 0 ? 0x400 : 0) |
						(definition->m_unk0xf4 != 0 ? 0x210 : 0) | (definition->m_distanceFlag != 0 ? 0x100 : 0) |
						(definition->m_spatialFlag != 0 ? 0x80 : 0) | (definition->m_musicFlag != 0 ? 0x40 : 0) |
						(definition->m_unk0xec != 0 ? 0x10 : 0) | (definition->m_lowFlags & 0xf);

	source->m_unk0x90[0] = definition->m_audioValue0;
	source->m_unk0x90[1] = definition->m_audioValue1;
	source->m_unk0x90[2] = definition->m_audioValue2;
	source->m_unk0x90[3] = definition->m_audioValue3;
	source->m_unk0x90[4] = definition->m_audioValue4;
	source->m_unk0x90[5] = definition->m_audioValue5;

	if (definition->m_range >= 0.0f && definition->m_falloff >= 0.0f) {
		source->m_defaultRange = definition->m_range;
		source->m_defaultFalloff = definition->m_falloff;
	}

	source->m_unk0x30 = 1;
	source->m_unk0x40 = 1;
	source->m_unk0x2c = 1;
	source->m_unk0x34 = 0;
	source->m_unk0x3c = 0;

	AudioChannelRecord* channel = FindOrCreateAudioChannel(p_service, NULL, definition->m_audioPath);
	if (channel != NULL) {
		IDirectSoundBuffer* buffer =
			g_audioBackend->m_vtable->m_duplicatePlaybackBuffer(g_audioBackend, NULL, channel->m_playbackBuffer);
		source->m_playbackBuffer = buffer;
		source->m_unk0xcc = channel->m_unk0x94;
		source->m_channelResource = channel;
		++channel->m_referenceCount;
	}
}

// STUB: LEGORACERS2 0x004b01e0
void __fastcall ReleaseAudioSourceRecord(AudioService*, void*, AudioSourceRecord* p_source)
{
	if (p_source != NULL) {
		ReleaseAudioSourceRecordResources(p_source, NULL);
	}
}

// STUB: LEGORACERS2 0x004b01f0
void __fastcall RemoveAudioSource(AudioService* p_service, void*, LegoU32 p_sourceId)
{
	if (p_sourceId == 0) {
		return;
	}
	for (LegoU32 i = 0; i < 512; ++i) {
		AudioSourceRecord* source = &p_service->m_sources[i];
		if (source->m_unk0x44 == p_sourceId) {
			ReleaseAudioSourceRecordResources(source, NULL);
		}
	}
}

// STUB: LEGORACERS2 0x004b0220
AudioSourceRecord* __fastcall FindAudioSource(AudioService* p_service, void*, LegoU32 p_sourceId, LPCSTR p_name)
{
	if (p_sourceId != 0) {
		for (LegoU32 i = 0; i < 512; ++i) {
			AudioSourceRecord* source = &p_service->m_sources[i];
			if (source->m_unk0x2c != 0 && source->m_unk0x44 == p_sourceId &&
				IsSameSourceName(source->m_unkName0x64, p_name) == TRUE) {
				return source;
			}
		}
	}
	return NULL;
}

// STUB: LEGORACERS2 0x004b0290
LegoU32 __fastcall ValidateAudioSource(AudioService* p_service, void*, LegoU32 p_sourceIndex)
{
	AudioSourceRecord* source = &p_service->m_sources[p_sourceIndex];
	DWORD status = 0;
	source->m_playbackBuffer->GetStatus(&status);
	if (status != 0) {
		ReportFatalError("Invalid Parameter");
	}
	return FALSE;
}

// STUB: LEGORACERS2 0x004b0320
AudioSourceRecord* __fastcall FindAvailableAudioSourceRecord(AudioService* p_service, void*)
{
	for (LegoU32 i = 0; i < 512; ++i) {
		AudioSourceRecord* source = &p_service->m_sources[i];
		if (source->m_unk0x2c == 0) {
			return source;
		}
	}
	return NULL;
}
