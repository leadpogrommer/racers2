#include "audioformat.h"

namespace
{

LegoU32 ReadLittleEndianDword(const LegoU8* p_bytes)
{
	return (LegoU32) p_bytes[0] | ((LegoU32) p_bytes[1] << 8) | ((LegoU32) p_bytes[2] << 16) |
		   ((LegoU32) p_bytes[3] << 24);
}

LegoU16 ReadBigEndianWord(const LegoU8* p_bytes)
{
	return (LegoU16) (((LegoU16) p_bytes[0] << 8) | p_bytes[1]);
}

LegoU32 MakeFourCC(LegoU8 p_a, LegoU8 p_b, LegoU8 p_c, LegoU8 p_d)
{
	return (LegoU32) p_a | ((LegoU32) p_b << 8) | ((LegoU32) p_c << 16) | ((LegoU32) p_d << 24);
}

} // namespace

// STUB: LEGORACERS2 0x00502d10
LegoBool32 ParseAiffAudioData(
	const void* p_file,
	WAVEFORMATEX** p_format,
	const LegoU8** p_sampleData,
	LegoU32* p_sampleByteCount,
	LegoU32* p_bitsPerSample
)
{
	const LegoU8* bytes = (const LegoU8*) p_file;
	const LegoU8* commonChunk = NULL;
	const LegoU8* soundChunk = NULL;
	LegoS32 soundByteCount = 0;
	LegoS32 bytesRemaining;

	if (p_format != NULL) {
		*p_format = NULL;
	}
	if (p_sampleData != NULL) {
		*p_sampleData = NULL;
	}
	if (p_sampleByteCount != NULL) {
		*p_sampleByteCount = 0;
	}
	if (p_bitsPerSample != NULL) {
		*p_bitsPerSample = 0;
	}

	if (ReadLittleEndianDword(bytes) != MakeFourCC('F', 'O', 'R', 'M') ||
		ReadLittleEndianDword(bytes + 8) != MakeFourCC('A', 'I', 'F', 'F')) {
		return FALSE;
	}

	bytesRemaining = (LegoS32) SwapBigEndianDword(ReadLittleEndianDword(bytes + 4)) - 4;
	const LegoU8* chunk = bytes + 12;
	if (bytesRemaining < 0) {
		return FALSE;
	}

	for (;;) {
		bytesRemaining -= 8;
		if (bytesRemaining < 0) {
			break;
		}

		LegoU32 chunkByteCount = SwapBigEndianDword(ReadLittleEndianDword(chunk + 4));
		if ((LegoS32) chunkByteCount < 0 || bytesRemaining < (LegoS32) chunkByteCount) {
			break;
		}

		const LegoU8* chunkData = chunk + 8;
		LegoU32 chunkType = ReadLittleEndianDword(chunk);
		bytesRemaining -= (LegoS32) chunkByteCount;
		if (chunkType == MakeFourCC('C', 'O', 'M', 'M') && chunkByteCount > 0x11 && commonChunk == NULL) {
			commonChunk = chunkData;
		}
		else if (chunkType == MakeFourCC('S', 'S', 'N', 'D') && soundChunk == NULL) {
			soundByteCount = (LegoS32) chunkByteCount - 8;
			soundChunk = chunkData;
		}

		chunk = chunkData + chunkByteCount;
	}

	if (commonChunk == NULL || soundChunk == NULL || soundByteCount <= 0) {
		return FALSE;
	}

	LegoU32 sampleRate = ((LegoU32) ReadBigEndianWord(commonChunk + 10) << (commonChunk[9] & 0x1f)) >> 14;
	LegoU16 channels = ReadBigEndianWord(commonChunk);
	LegoU16 bitsPerSample = ReadBigEndianWord(commonChunk + 6);
	if (channels != 1 || (bitsPerSample != 8 && bitsPerSample != 16)) {
		return FALSE;
	}

	g_aiffPcmFormat.wFormatTag = WAVE_FORMAT_PCM;
	g_aiffPcmFormat.nChannels = 1;
	g_aiffPcmFormat.nSamplesPerSec = sampleRate;
	LegoS32 averageByteRate = (LegoS32) ((LegoU32) bitsPerSample * sampleRate);
	averageByteRate = (averageByteRate + ((averageByteRate >> 31) & 7)) >> 3;
	g_aiffPcmFormat.nAvgBytesPerSec = (LegoU32) averageByteRate;
	LegoS32 blockAlign = (LegoS32) bitsPerSample;
	blockAlign = (blockAlign + ((blockAlign >> 31) & 7)) >> 3;
	g_aiffPcmFormat.nBlockAlign = (LegoU16) blockAlign;
	g_aiffPcmFormat.wBitsPerSample = bitsPerSample;
	g_aiffPcmFormat.cbSize = 0;

	if (p_format != NULL) {
		*p_format = &g_aiffPcmFormat;
	}
	if (p_sampleData != NULL) {
		*p_sampleData = soundChunk;
	}
	if (p_sampleByteCount != NULL) {
		*p_sampleByteCount = (LegoU32) soundByteCount;
	}
	if (p_bitsPerSample != NULL) {
		*p_bitsPerSample = bitsPerSample;
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x00502f00
LegoU32 SwapBigEndianDword(LegoU32 p_value)
{
	return ((p_value & 0x000000ff) << 24) | ((p_value & 0x0000ff00) << 8) | ((p_value & 0x00ff0000) >> 8) |
		   ((p_value & 0xff000000) >> 24);
}

// STUB: LEGORACERS2 0x00502f20
LegoU32 SwapBigEndianWord(LegoU32 p_value)
{
	return ((p_value & 0xff) << 8) | ((p_value >> 8) & 0xff);
}

// STUB: LEGORACERS2 0x00502f40
void ConvertBigEndianPcm(LegoU8* p_destination, const LegoU8* p_source, LegoU32 p_byteCount, LegoS32 p_bitsPerSample)
{
	if (p_bitsPerSample == 8) {
		if ((LegoS32) p_byteCount > 0) {
			for (LegoU32 i = 0; i < p_byteCount; ++i) {
				p_destination[i] = p_source[i] ^ 0x80;
			}
		}
	}
	else if (p_bitsPerSample == 16) {
		if ((LegoS32) p_byteCount > 0) {
			for (LegoU32 i = 0; i < p_byteCount; ++i) {
				p_destination[i] = p_source[i ^ 1];
			}
		}
	}
	else {
		// Retail copies forward in DWORD groups, then the remaining bytes.
		for (LegoU32 count = p_byteCount >> 2; count != 0; --count) {
			*(LegoU32*) p_destination = *(const LegoU32*) p_source;
			p_destination += 4;
			p_source += 4;
		}
		for (LegoU32 remaining = p_byteCount & 3; remaining != 0; --remaining) {
			*p_destination++ = *p_source++;
		}
	}
}

// STUB: LEGORACERS2 0x00502fc0
LegoBool32 ParseWaveOrAiffAudioData(
	const void* p_file,
	WAVEFORMATEX** p_format,
	const LegoU8** p_sampleData,
	LegoU32* p_sampleByteCount,
	LegoU32* p_bitsPerSample
)
{
	const LegoU8* bytes = (const LegoU8*) p_file;
	if (p_format != NULL) {
		*p_format = NULL;
	}
	if (p_sampleData != NULL) {
		*p_sampleData = NULL;
	}
	if (p_sampleByteCount != NULL) {
		*p_sampleByteCount = 0;
	}
	if (p_bitsPerSample != NULL) {
		*p_bitsPerSample = 0;
	}

	if (ReadLittleEndianDword(bytes) == MakeFourCC('R', 'I', 'F', 'F')) {
		return ParseWaveAudioData(p_file, p_format, p_sampleData, p_sampleByteCount, p_bitsPerSample);
	}
	if (ReadLittleEndianDword(bytes) == MakeFourCC('F', 'O', 'R', 'M')) {
		return ParseAiffAudioData(p_file, p_format, p_sampleData, p_sampleByteCount, p_bitsPerSample);
	}
	return FALSE;
}

// STUB: LEGORACERS2 0x00503050
LegoBool32 ParseWaveAudioData(
	const void* p_file,
	WAVEFORMATEX** p_format,
	const LegoU8** p_sampleData,
	LegoU32* p_sampleByteCount,
	LegoU32* p_bitsPerSample
)
{
	const LegoU8* bytes = (const LegoU8*) p_file;
	if (p_format != NULL) {
		*p_format = NULL;
	}
	if (p_sampleData != NULL) {
		*p_sampleData = NULL;
	}
	if (p_sampleByteCount != NULL) {
		*p_sampleByteCount = 0;
	}
	if (p_bitsPerSample != NULL) {
		*p_bitsPerSample = 0;
	}

	LegoU32 containerByteCount = ReadLittleEndianDword(bytes + 4);
	if (ReadLittleEndianDword(bytes) != MakeFourCC('R', 'I', 'F', 'F') ||
		ReadLittleEndianDword(bytes + 8) != MakeFourCC('W', 'A', 'V', 'E')) {
		return FALSE;
	}
	if (containerByteCount == 0) {
		containerByteCount = 0x20;
	}

	const LegoU8* chunk = bytes + 12;
	const LegoU8* end = chunk + containerByteCount - 4;
	if (chunk >= end) {
		return FALSE;
	}

	while (chunk < end) {
		LegoU32 chunkType = ReadLittleEndianDword(chunk);
		LegoU32 chunkByteCount = ReadLittleEndianDword(chunk + 4);
		const LegoU8* chunkData = chunk + 8;

		if (chunkType == MakeFourCC('f', 'm', 't', ' ') && p_format != NULL && *p_format == NULL) {
			if (chunkByteCount < 0x0e) {
				return FALSE;
			}
			*p_format = (WAVEFORMATEX*) chunkData;
			if (p_sampleData == NULL || *p_sampleData != NULL) {
				if (p_sampleByteCount == NULL) {
					return TRUE;
				}
				if (*p_sampleByteCount != 0) {
					return TRUE;
				}
			}
		}
		else if (
			chunkType == MakeFourCC('d', 'a', 't', 'a') &&
			((p_sampleData != NULL && *p_sampleData == NULL) || (p_sampleByteCount != NULL && *p_sampleByteCount == 0))
		) {
			if (p_sampleData != NULL) {
				*p_sampleData = chunkData;
			}
			if (p_sampleByteCount != NULL) {
				*p_sampleByteCount = chunkByteCount;
			}
			if (p_format == NULL || *p_format != NULL) {
				return TRUE;
			}
		}

		chunk = chunkData + ((chunkByteCount + 1) & ~1);
	}
	return FALSE;
}

// GLOBAL: LEGORACERS2 0x006fc190
WAVEFORMATEX g_aiffPcmFormat;
