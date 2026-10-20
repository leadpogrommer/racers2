#include "game/runtime/gamerenderaudio.h"

#include <string.h>

// GLOBAL: LEGORACERS2 0x00684490
AudioFrameStateRecord g_audioFrameRecordA;

// GLOBAL: LEGORACERS2 0x006844b0
AudioFrameStateRecord g_audioFrameRecordB;

// GLOBAL: LEGORACERS2 0x006843f0
LegoU32 g_audioFrameSourceValues[3];

// GLOBAL: LEGORACERS2 0x00684400
LegoU32 g_audioFrameCopiedValues[15];

// GLOBAL: LEGORACERS2 0x00684440
LegoU32 g_audioFrameCopySource[15];

// GLOBAL: LEGORACERS2 0x00684480
LegoU32 g_audioFrameCurrentValues[3];

// GLOBAL: LEGORACERS2 0x006844d4
void* g_unk0x006844d4;

// GLOBAL: LEGORACERS2 0x006844d8
LegoU32 g_unk0x006844d8;

// GLOBAL: LEGORACERS2 0x006844dc
LegoU32 g_unk0x006844dc;

namespace
{

void SetAudioFrameRecord(AudioFrameStateRecord* p_record, LegoU32 p_value0, LegoU32 p_value1, LegoU32 p_value2)
{
	// 0x005028d0 stores the third, first, and second arguments at +0xc, +0,
	// and +4, respectively.
	p_record->m_value2 = p_value2;
	p_record->m_value0 = p_value0;
	p_record->m_value1 = p_value1;
}

void UpdateAudioFrameState(LegoU32 p_copyCurrentValues)
{
	g_audioFrameRecordB.m_unk0x18 = g_unk0x006844d8;
	g_audioFrameRecordB.m_unk0x1c = g_unk0x006844dc;
	memcpy(g_audioFrameCopiedValues, g_audioFrameCopySource, sizeof(g_audioFrameCopiedValues));
	g_audioFrameRecordB.m_unk0x14 = g_audioFrameRecordA.m_unk0x14;
	if (p_copyCurrentValues != 0) {
		memcpy(g_audioFrameCurrentValues, g_audioFrameSourceValues, sizeof(g_audioFrameCurrentValues));
		g_audioFrameRecordB.m_unk0x1c = 1;
	}
}

} // namespace

// STUB: LEGORACERS2 0x004e8830
void FinishFrameAudioState()
{
	if (g_unk0x006844d4 == NULL) {
		return;
	}

	UpdateAudioFrameState(1);
	SetAudioFrameRecord(&g_audioFrameRecordA, 0, 0, 0);
	SetAudioFrameRecord(&g_audioFrameRecordB, 0, 0, 0);
	if (g_unk0x006844d4 == NULL) {
		return;
	}

	LegoU8* nested = (LegoU8*) *(void**) ((LegoU8*) g_unk0x006844d4 + 0x0c);
	nested = (LegoU8*) *(void**) (nested + 0x10);
	*(LegoU16*) nested = 0;
	*(LegoU16*) (nested + 2) = 0;
	LegoU8* state = (LegoU8*) *(void**) (nested + 0x10);
	*(LegoU32*) (state + 0x14) = *(LegoU32*) (state + 0x10);
}
