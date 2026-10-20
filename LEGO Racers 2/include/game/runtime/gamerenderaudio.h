#ifndef LR2_GAMERENDERAUDIO_H
#define LR2_GAMERENDERAUDIO_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

struct AudioFrameStateRecord {
	LegoU32 m_value0;
	LegoU32 m_value1;
	LegoU32 m_unk0x08;
	LegoU32 m_value2;
	LegoU32 m_unk0x10;
	LegoU32 m_unk0x14;
	LegoU32 m_unk0x18;
	LegoU32 m_unk0x1c;
};

DECOMP_SIZE_ASSERT(AudioFrameStateRecord, 0x20);
DECOMP_STATIC_ASSERT(offsetof(AudioFrameStateRecord, m_value2) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(AudioFrameStateRecord, m_unk0x14) == 0x14);
DECOMP_STATIC_ASSERT(offsetof(AudioFrameStateRecord, m_unk0x18) == 0x18);
DECOMP_STATIC_ASSERT(offsetof(AudioFrameStateRecord, m_unk0x1c) == 0x1c);

extern AudioFrameStateRecord g_audioFrameRecordA;
extern AudioFrameStateRecord g_audioFrameRecordB;
extern LegoU32 g_audioFrameSourceValues[3];
extern LegoU32 g_audioFrameCopiedValues[15];
extern LegoU32 g_audioFrameCopySource[15];
extern LegoU32 g_audioFrameCurrentValues[3];
extern void* g_unk0x006844d4;
extern LegoU32 g_unk0x006844d8;
extern LegoU32 g_unk0x006844dc;

void FinishFrameAudioState();

#endif // LR2_GAMERENDERAUDIO_H
