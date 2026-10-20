#ifndef LR2_OVERLAYRECORDS_H
#define LR2_OVERLAYRECORDS_H

#include "types.h"

// Retail 0x004c4f30 compares a key prefix against a UTF-16 string. The
// receiver is retained for the original thiscall ABI; the body does not use it.
LegoBool32 __fastcall CompareOverlayTextPrefix(
	void* p_glyphManager,
	void* p_unusedEcxScratch,
	const LegoU16* p_key,
	const LegoU16* p_text,
	LegoU32* p_matchedLength
);

LegoS32 __fastcall FindOverlayGlyphSequence(
	void* p_glyphManager,
	void* p_unusedEcxScratch,
	const LegoU16* p_key,
	LegoU32* p_matchedLength
);

LegoBool32 __fastcall ExpandOverlayGlyphSequence(
	void* p_glyphManager,
	void* p_unusedEcxScratch,
	const LegoU16* p_key,
	LegoU32* p_unusedMatchedLength,
	LegoU16** p_output
);

#endif // LR2_OVERLAYRECORDS_H
