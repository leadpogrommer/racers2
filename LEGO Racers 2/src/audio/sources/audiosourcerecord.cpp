#include "audio/service/audioservice.h"
#include "memory/memoryheap.h"

extern "C" int __cdecl _purecall(void);

AudioSourceRecord* __fastcall DeleteBaseAudioSourceRecord(AudioSourceRecord* p_record, void*, LegoU32 p_delete);
AudioSourceRecord* __fastcall DeleteAudioSourceRecord(AudioSourceRecord* p_record, void*, LegoU32 p_delete);
void __fastcall DisposeAudioSourceRecord(AudioSourceRecord* p_record, void*);
void __fastcall ReleaseAudioSourceRecordResources(AudioSourceRecord* p_record, void*);
void __fastcall ReleaseAudioSourcePlayback(AudioSourceRecord* p_record, void*);
LegoU32 __fastcall GetAudioSourceRecordIndex(AudioSourceRecord* p_record, void*);
void __fastcall SetAudioSourceRecordValues0x10(AudioSourceRecord* p_record, void*, LegoU32, LegoU32, LegoU32);
void __fastcall SetAudioSourceRecordValues0x04(AudioSourceRecord* p_record, void*, LegoU32, LegoU32, LegoU32);
void __fastcall SetAudioSourceRecordValues0x1c(AudioSourceRecord* p_record, void*, LegoU32, LegoU32, LegoU32);
void __fastcall SetAudioSourceRecordFalloff(AudioSourceRecord* p_record, void*, LegoU32, LegoFloat);
void __fastcall SetAudioSourceRecordFlagsLow(AudioSourceRecord* p_record, void*, LegoU32 p_value);
void __fastcall SetAudioSourceRecordFlagsHigh(AudioSourceRecord* p_record, void*, LegoU32 p_value);
void __fastcall SetAudioSourceRecordValues0x90(AudioSourceRecord* p_record, void*, LegoU32 p_first, LegoU32 p_second);
void __fastcall SetAudioSourceRecordValues0x98(AudioSourceRecord* p_record, void*, LegoU32 p_first, LegoU32 p_second);
void __fastcall SetAudioSourceRecordValues0xa0(AudioSourceRecord* p_record, void*, LegoU32 p_first, LegoU32 p_second);
void __fastcall SetAudioSourceRecordGains(AudioSourceRecord* p_record, void*, LegoFloat p_first, LegoFloat p_second);
void __fastcall SetAudioSourceRecordMaximumGain(AudioSourceRecord* p_record, void*, LegoFloat p_gain);
void __fastcall ResetAudioSourcePlaybackState(AudioSourceRecord* p_record, void*);
void __fastcall ClearAudioSourceFlag(AudioSourceRecord* p_record, void*);
void __fastcall SetAudioSourceEnabled(AudioSourceRecord* p_record, void*, LegoU32 p_enabled);
void __fastcall DestroyAudioChannelRecordResources(AudioChannelRecord* p_record, void*);
void __fastcall ReleaseAudioChannelRecordBuffer(AudioChannelRecord* p_record, void*);
void __fastcall InitializeAudioChannelRecord(AudioChannelRecord* p_record, void*);

// GLOBAL: LEGORACERS2 0x00535b8c
const AudioSourceRecordDispatch g_audioSourceRecordBaseDispatch = {
	DeleteBaseAudioSourceRecord,
	(AudioSourceRecordDispatch::SetOneWord) _purecall,
	(AudioSourceRecordDispatch::SetOneWord) _purecall,
	(AudioSourceRecordDispatch::SetTwoWords) _purecall,
	(AudioSourceRecordDispatch::SetTwoWords) _purecall,
	(AudioSourceRecordDispatch::SetTwoWords) _purecall,
	(AudioSourceRecordDispatch::SetTwoFloats) _purecall,
	(AudioSourceRecordDispatch::SetOneFloat) _purecall,
	(AudioSourceRecordDispatch::SetThreeWords) _purecall,
	(AudioSourceRecordDispatch::SetThreeWords) _purecall,
	(AudioSourceRecordDispatch::SetThreeWords) _purecall,
	(AudioSourceRecordDispatch::SetFloatWithUnusedFirst) _purecall,
	(AudioSourceRecordDispatch::GetIndex) _purecall,
	(AudioSourceRecordDispatch::Action) _purecall,
	(AudioSourceRecordDispatch::Action) _purecall,
	(AudioSourceRecordDispatch::SetOneWord) _purecall
};

// GLOBAL: LEGORACERS2 0x00535b4c
const AudioSourceRecordDispatch g_audioSourceRecordOperationalDispatch = {
	DeleteAudioSourceRecord,
	SetAudioSourceRecordFlagsLow,
	SetAudioSourceRecordFlagsHigh,
	SetAudioSourceRecordValues0x90,
	SetAudioSourceRecordValues0x98,
	SetAudioSourceRecordValues0xa0,
	SetAudioSourceRecordGains,
	SetAudioSourceRecordMaximumGain,
	SetAudioSourceRecordValues0x10,
	SetAudioSourceRecordValues0x04,
	SetAudioSourceRecordValues0x1c,
	SetAudioSourceRecordFalloff,
	GetAudioSourceRecordIndex,
	ResetAudioSourcePlaybackState,
	ClearAudioSourceFlag,
	SetAudioSourceEnabled
};

// FUNCTION: LEGORACERS2 0x004b0370
AudioSourceRecord* __fastcall DeleteBaseAudioSourceRecord(AudioSourceRecord* p_record, void*, LegoU32 p_delete)
{
	p_record->m_vtable = &g_audioSourceRecordBaseDispatch;
	if ((p_delete & 1) != 0) {
		FreeHeapMemory(p_record);
	}
	return p_record;
}

// FUNCTION: LEGORACERS2 0x004b0390
AudioSourceRecord* __fastcall DeleteAudioSourceRecord(AudioSourceRecord* p_record, void*, LegoU32 p_delete)
{
	DisposeAudioSourceRecord(p_record, NULL);
	if ((p_delete & 1) != 0) {
		FreeHeapMemory(p_record);
	}
	return p_record;
}

// STUB: LEGORACERS2 0x004b04c0
void __fastcall DisposeAudioSourceRecord(AudioSourceRecord* p_record, void*)
{
	p_record->m_vtable = &g_audioSourceRecordOperationalDispatch;
	ReleaseAudioSourceRecordResources(p_record, NULL);
	p_record->m_vtable = &g_audioSourceRecordBaseDispatch;
}

// STUB: LEGORACERS2 0x004b04e0
void __fastcall ReleaseAudioSourceRecordResources(AudioSourceRecord* p_record, void*)
{
	ReleaseAudioSourcePlayback(p_record, NULL);
	InitializeAudioSourceRecordFields(p_record, NULL);
}

// STUB: LEGORACERS2 0x004b0500
void __fastcall ReleaseAudioSourcePlayback(AudioSourceRecord* p_record, void*)
{
	if (p_record->m_playbackBuffer != NULL) {
		g_audioBackend->m_vtable->m_releaseSourceBuffer(g_audioBackend, NULL, p_record);
		AudioChannelRecord* channel = p_record->m_channelResource;
		p_record->m_playbackBuffer = NULL;
		if (channel != NULL) {
			--channel->m_referenceCount;
			if (channel->m_referenceCount == 0) {
				DestroyAudioChannelRecordResources(channel, NULL);
				p_record->m_channelResource = NULL;
			}
		}
	}
}

// FUNCTION: LEGORACERS2 0x004b0550
LegoU32 __fastcall GetAudioSourceRecordIndex(AudioSourceRecord* p_record, void*)
{
	return p_record->m_recordIndex;
}

// FUNCTION: LEGORACERS2 0x004b0560
void __fastcall SetAudioSourceRecordValues0x10(
	AudioSourceRecord* p_record,
	void*,
	LegoU32 p_first,
	LegoU32 p_second,
	LegoU32 p_third
)
{
	p_record->m_unk0x10 = p_first;
	p_record->m_unk0x14 = p_second;
	p_record->m_unk0x18 = p_third;
}

// FUNCTION: LEGORACERS2 0x004b0580
void __fastcall SetAudioSourceRecordValues0x04(
	AudioSourceRecord* p_record,
	void*,
	LegoU32 p_first,
	LegoU32 p_second,
	LegoU32 p_third
)
{
	p_record->m_unk0x04 = p_first;
	p_record->m_unk0x08 = p_second;
	p_record->m_unk0x0c = p_third;
}

// FUNCTION: LEGORACERS2 0x004b05a0
void __fastcall SetAudioSourceRecordValues0x1c(
	AudioSourceRecord* p_record,
	void*,
	LegoU32 p_first,
	LegoU32 p_second,
	LegoU32 p_third
)
{
	p_record->m_unk0x1c = p_first;
	p_record->m_unk0x20 = p_second;
	p_record->m_unk0x24 = p_third;
}

// FUNCTION: LEGORACERS2 0x004b05c0
void __fastcall SetAudioSourceRecordFalloff(AudioSourceRecord* p_record, void*, LegoU32, LegoFloat p_falloff)
{
	p_record->m_defaultFalloff = p_falloff;
}

// FUNCTION: LEGORACERS2 0x004b05d0
void __fastcall SetAudioSourceRecordFlagsLow(AudioSourceRecord* p_record, void*, LegoU32 p_value)
{
	p_record->m_unk0xb8 = (p_record->m_unk0xb8 & 0xfffffff0) | p_value;
}

// FUNCTION: LEGORACERS2 0x004b05f0
void __fastcall SetAudioSourceRecordFlagsHigh(AudioSourceRecord* p_record, void*, LegoU32 p_value)
{
	p_record->m_unk0xb8 = (p_record->m_unk0xb8 & 0xf) | p_value;
}

// FUNCTION: LEGORACERS2 0x004b0610
void __fastcall SetAudioSourceRecordValues0x90(AudioSourceRecord* p_record, void*, LegoU32 p_first, LegoU32 p_second)
{
	p_record->m_unk0x90[0] = p_first;
	p_record->m_unk0x90[1] = p_second;
}

// FUNCTION: LEGORACERS2 0x004b0630
void __fastcall SetAudioSourceRecordValues0x98(AudioSourceRecord* p_record, void*, LegoU32 p_first, LegoU32 p_second)
{
	p_record->m_unk0x90[2] = p_first;
	p_record->m_unk0x90[3] = p_second;
}

// FUNCTION: LEGORACERS2 0x004b0650
void __fastcall SetAudioSourceRecordValues0xa0(AudioSourceRecord* p_record, void*, LegoU32 p_first, LegoU32 p_second)
{
	p_record->m_unk0x90[4] = p_first;
	p_record->m_unk0x90[5] = p_second;
}

// STUB: LEGORACERS2 0x004b0670
void __fastcall SetAudioSourceRecordGains(AudioSourceRecord* p_record, void*, LegoFloat p_first, LegoFloat p_second)
{
	if (!(p_first >= 0.0f)) {
		p_record->m_unk0x80 = 0.0f;
	}
	else if (p_first <= 1.0f) {
		p_record->m_unk0x80 = p_first;
	}
	else {
		p_record->m_unk0x80 = 1.0f;
	}

	if (p_second < 0.0f) {
		p_record->m_unk0x84 = 0.0f;
	}
	else if (p_second <= 1.0f) {
		p_record->m_unk0x84 = p_second;
	}
	else {
		p_record->m_unk0x84 = 1.0f;
	}
}

// STUB: LEGORACERS2 0x004b0700
void __fastcall SetAudioSourceRecordMaximumGain(AudioSourceRecord* p_record, void*, LegoFloat p_gain)
{
	if (p_gain < 0.0f) {
		p_record->m_unk0x7c = 0.0f;
	}
	else if (p_gain > 4.0f) {
		p_record->m_unk0x7c = 4.0f;
	}
	else {
		p_record->m_unk0x7c = p_gain;
	}
}

// FUNCTION: LEGORACERS2 0x004b0750
void __fastcall ResetAudioSourcePlaybackState(AudioSourceRecord* p_record, void*)
{
	p_record->m_unk0x34 = 0;
	p_record->m_unk0xb8 |= 0x10;
	p_record->m_unk0x30 = 1;
	p_record->m_unk0x3c = 1;
}

// FUNCTION: LEGORACERS2 0x004b0780
void __fastcall ClearAudioSourceFlag(AudioSourceRecord* p_record, void*)
{
	p_record->m_unk0xb8 &= ~0x10;
}

// FUNCTION: LEGORACERS2 0x004b0790
void __fastcall SetAudioSourceEnabled(AudioSourceRecord* p_record, void*, LegoU32 p_enabled)
{
	if (p_enabled != 0) {
		p_record->m_unk0xb8 |= 0x100;
	}
	else {
		p_record->m_unk0xb8 &= ~0x100;
	}
}
// STUB: LEGORACERS2 0x004b0a90
void __fastcall DestroyAudioChannelRecordResources(AudioChannelRecord* p_record, void*)
{
	if (p_record->m_playbackBuffer != NULL) {
		g_audioBackend->m_vtable->m_releaseBufferPointer(g_audioBackend, NULL, &p_record->m_playbackBuffer);
	}
	ReleaseAudioChannelRecordBuffer(p_record, NULL);
	InitializeAudioChannelRecord(p_record, NULL);
}

// FUNCTION: LEGORACERS2 0x004b0bd0
void __fastcall ReleaseAudioChannelRecordBuffer(AudioChannelRecord* p_record, void*)
{
	if (p_record->m_cachedBuffer != NULL) {
		MemoryHeap* heap = GetMemoryHeap();
		heap->m_useCRT = TRUE;
		FreeHeapMemory(p_record->m_cachedBuffer);
		heap = GetMemoryHeap();
		heap->m_useCRT = FALSE;
		p_record->m_cachedBuffer = NULL;
		p_record->m_unk0x88 = 0;
	}
}
