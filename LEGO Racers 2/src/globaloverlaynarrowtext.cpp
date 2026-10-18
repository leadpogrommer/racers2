#include "globaloverlaymanager.h"

// Shared conversion scratch at retail654e20. The copied text has no bound in
// retail. Reserve 0x400 bytes within the otherwise unclaimed BSS gap before
// GlobalOverlayManager655228; the original capacity remains unproven.
// GLOBAL: LEGORACERS2 0x00654e20
LegoU16 g_overlayNarrowTextBuffer[512];

// FUNCTION: LEGORACERS2 0x004b3b70
void GlobalOverlayRecord::InitializeNarrowText(const LegoChar* p_text, LegoU32 p_flags, LegoFloat p_time)
{
	const LegoU8* source = (const LegoU8*) p_text;
	LegoU16* destination = g_overlayNarrowTextBuffer;
	do {
		*destination++ = *source;
	} while (*source++ != 0);
	InitializeText(g_overlayNarrowTextBuffer, p_flags, p_time);
}
