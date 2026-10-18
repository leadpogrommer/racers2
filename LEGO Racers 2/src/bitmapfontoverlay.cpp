#include "bitmapfont.h"
#include "globaloverlayglyph.h"
#include "globaloverlaymanager.h"

// STUB: LEGORACERS2 0x004c3e30
LegoBool32 BitmapFont::GetTextAdvance(const LegoU16* p_text, LegoU32* p_matchedLength, LegoFloat* p_advance)
{
	if (p_text[0] == 0x20) {
		*p_advance = *(LegoFloat*) ((LegoU8*) this + 0x28) * m_scaleX;
		return TRUE;
	}

	GlobalOverlayGlyphReplacementManagerView* replacements =
		(GlobalOverlayGlyphReplacementManagerView*) g_globalOverlayManager.m_unk0x2494c;
	if (replacements != NULL &&
		HasGlobalOverlayGlyphReplacement(replacements, NULL, p_text, p_matchedLength) != FALSE) {
		*p_advance = MeasureGlobalOverlayGlyphReplacement(replacements, NULL, p_text, p_matchedLength, this);
		return TRUE;
	}

	BitmapFontGlyph* glyph = &m_glyphs[m_characterMap[p_text[0]]];
	*p_advance = (glyph->m_width + m_spacing) * m_scaleX;
	return TRUE;
}
