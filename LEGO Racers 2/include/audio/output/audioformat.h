#ifndef LR2_AUDIOFORMAT_H
#define LR2_AUDIOFORMAT_H

#include "decomp.h"
#include "directx8.h"
#include "types.h"

LegoU32 SwapBigEndianDword(LegoU32 p_value);
LegoU32 SwapBigEndianWord(LegoU32 p_value);

void ConvertBigEndianPcm(LegoU8* p_destination, const LegoU8* p_source, LegoU32 p_byteCount, LegoS32 p_bitsPerSample);

LegoBool32 ParseWaveAudioData(
	const void* p_file,
	WAVEFORMATEX** p_format,
	const LegoU8** p_sampleData,
	LegoU32* p_sampleByteCount,
	LegoU32* p_bitsPerSample
);

LegoBool32 ParseAiffAudioData(
	const void* p_file,
	WAVEFORMATEX** p_format,
	const LegoU8** p_sampleData,
	LegoU32* p_sampleByteCount,
	LegoU32* p_bitsPerSample
);

LegoBool32 ParseWaveOrAiffAudioData(
	const void* p_file,
	WAVEFORMATEX** p_format,
	const LegoU8** p_sampleData,
	LegoU32* p_sampleByteCount,
	LegoU32* p_bitsPerSample
);

extern WAVEFORMATEX g_aiffPcmFormat;

#endif // LR2_AUDIOFORMAT_H
