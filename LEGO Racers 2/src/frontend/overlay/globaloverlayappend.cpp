#include "frontend/overlay/globaloverlayglyph.h"
#include "frontend/overlay/globaloverlaymanager.h"

// These are separate shared scratch areas, not aliases of the narrow-text
// initialization scratch at 654e20. Retail copies without a bound; the original
// declarations' capacities are unproven. Reserve the available 0x200-byte gaps.
// GLOBAL: LEGORACERS2 0x00654c20
static LegoU16 g_overlayAppendNarrowBuffer[256];
// GLOBAL: LEGORACERS2 0x00655020
static LegoU16 g_overlayAppendBuffer[256];

// STUB: LEGORACERS2 0x004b3bb0
void GlobalOverlayRecord::AppendText(const LegoU16* p_text, LegoU32 p_addSpace)
{
	LegoU32 nextGlyph = m_textLength == 0 ? 0xffffffff : m_characterAnimationIndices[0];
	const LegoU16* source = m_text;
	LegoU16* destination = g_overlayAppendBuffer;
	GlobalOverlayGlyphReplacementManagerView* glyphManager =
		(GlobalOverlayGlyphReplacementManagerView*) g_globalOverlayManager.m_unk0x2494c;
	LegoS32 glyph = 0;
	LegoU32 offset = 0;
	while (*source != 0) {
		if (offset == nextGlyph) {
			++glyph;
			nextGlyph = glyph < m_textLength ? m_characterAnimationIndices[glyph] : 0xffffffff;
			const LegoU16* keyword = glyphManager->GetIconKeyword(*source - 0xd903);
			while (*keyword != 0) {
				*destination++ = *keyword++;
			}
		}
		else {
			*destination++ = *source;
		}
		++source;
		++offset;
	}
	if (p_addSpace != 0) {
		*destination++ = ' ';
	}
	do {
		*destination++ = *p_text;
	} while (*p_text++ != 0);
	InitializeText(g_overlayAppendBuffer, m_animationFlags, m_elapsed);
}

// FUNCTION: LEGORACERS2 0x004b3d00
void GlobalOverlayRecord::AppendNarrowText(const LegoChar* p_text, LegoU32 p_addSpace)
{
	const LegoU8* source = (const LegoU8*) p_text;
	LegoU16* destination = g_overlayAppendNarrowBuffer;
	do {
		*destination++ = *source;
	} while (*source++ != 0);
	AppendText(g_overlayAppendNarrowBuffer, p_addSpace);
}

// FUNCTION: LEGORACERS2 0x004c5100
const LegoU16* GlobalOverlayGlyphReplacementManagerView::GetIconKeyword(LegoU32 p_index) const
{
	return m_entries[p_index]->m_text;
}
