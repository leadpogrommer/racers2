#include "audiocommentary.h"

// These forward DWORD/tail-byte operations are retained pending provenance
// identification. A bounded pinned LIBCMT scan found no exact candidates.
// STUB: LEGORACERS2 0x00503390
void CopyAudioBytes(void* p_destination, const void* p_source, DWORD p_bytes)
{
	LegoU32* destination = (LegoU32*) p_destination;
	const LegoU32* source = (const LegoU32*) p_source;
	DWORD words = p_bytes >> 2;
	while (words-- != 0) {
		*destination++ = *source++;
	}
	LegoU8* destinationBytes = (LegoU8*) destination;
	const LegoU8* sourceBytes = (const LegoU8*) source;
	DWORD bytes = p_bytes & 3;
	while (bytes-- != 0) {
		*destinationBytes++ = *sourceBytes++;
	}
}

// STUB: LEGORACERS2 0x005033c0
void ClearAudioBytes(void* p_destination, DWORD p_bytes)
{
	LegoU32* destination = (LegoU32*) p_destination;
	DWORD words = p_bytes >> 2;
	while (words-- != 0) {
		*destination++ = 0;
	}
	LegoU8* destinationBytes = (LegoU8*) destination;
	DWORD bytes = p_bytes & 3;
	while (bytes-- != 0) {
		*destinationBytes++ = 0;
	}
}
