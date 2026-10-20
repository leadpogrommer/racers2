#include "frontend/overlay/globaloverlayglyph.h"

#include "frontend/overlay/globaloverlaymanager.h"
#include "frontend/overlay/overlayrecords.h"

namespace
{

LegoFloat GetRecordGlyphOffsetScale(const GlobalOverlayRecord* p_record)
{
	if ((p_record->m_animationFlags & 8) != 0) {
		return p_record->m_scale;
	}
	return 1.0f;
}

} // namespace

// STUB: LEGORACERS2 0x004c4f70
LegoBool32 __fastcall HasGlobalOverlayGlyphReplacement(
	GlobalOverlayGlyphReplacementManagerView* p_manager,
	void*,
	const LegoU16* p_text,
	LegoU32* p_matchedLength
)
{
	for (LegoS32 index = 0; index < p_manager->m_entryCount; ++index) {
		if (CompareOverlayTextPrefix(p_manager, NULL, p_manager->m_entries[index]->m_text, p_text, p_matchedLength)) {
			return TRUE;
		}
	}
	return FALSE;
}

// STUB: LEGORACERS2 0x004c4fc0
void __fastcall ApplyGlobalOverlayGlyphReplacement(
	GlobalOverlayGlyphReplacementManagerView* p_manager,
	void*,
	LegoU32 p_glyphIndex,
	GlobalOverlayRecord* p_record,
	BitmapGlyphQuad* p_quad,
	LegoFloat* p_advance
)
{
	GlobalOverlayGlyphEntryView* entry = p_manager->m_entries[p_glyphIndex];
	LegoFloat scale = GetRecordGlyphOffsetScale(p_record);
	LegoFloat baseHeight =
		*(LegoFloat*) ((LegoU8*) p_record->m_font + 0x20) * *(LegoFloat*) ((LegoU8*) p_record->m_font + 0x18);
	LegoFloat scaledHeight = baseHeight * entry->m_heightScale;
	*p_advance = scaledHeight * entry->m_widthScale;
	LegoFloat topOffset = ((baseHeight - scaledHeight) * scale) * 0.5f;
	p_quad->m_top += topOffset;
	p_quad->m_right = p_quad->m_left + *p_advance;
	p_quad->m_bottom = p_quad->m_top + scaledHeight;
	LegoU32* destination = (LegoU32*) &p_quad->m_texture;
	const LegoU32* source = (const LegoU32*) &entry->m_texture;
	for (LegoU32 index = 0; index < 5; ++index) {
		destination[index] = source[index];
	}
}

// STUB: LEGORACERS2 0x004c5030
LegoFloat __fastcall MeasureGlobalOverlayGlyphReplacement(
	GlobalOverlayGlyphReplacementManagerView* p_manager,
	void*,
	const LegoU16* p_text,
	LegoU32* p_matchedLength,
	BitmapFont* p_font
)
{
	for (LegoS32 index = 0; index < p_manager->m_entryCount; ++index) {
		GlobalOverlayGlyphEntryView* entry = p_manager->m_entries[index];
		if (CompareOverlayTextPrefix(p_manager, NULL, entry->m_text, p_text, p_matchedLength)) {
			return *(LegoFloat*) ((LegoU8*) p_font + 0x20) * p_font->m_scaleY * entry->m_heightScale *
				   entry->m_widthScale;
		}
	}
	return 0.0f;
}

// STUB: LEGORACERS2 0x004c5090
LegoFloat __fastcall GetGlobalOverlayGlyphReplacementAdvance(
	GlobalOverlayGlyphReplacementManagerView* p_manager,
	void*,
	LegoU32 p_glyphIndex,
	BitmapFont* p_font
)
{
	GlobalOverlayGlyphEntryView* entry = p_manager->m_entries[p_glyphIndex];
	return *(LegoFloat*) ((LegoU8*) p_font + 0x20) * p_font->m_scaleY * entry->m_heightScale * entry->m_widthScale;
}
