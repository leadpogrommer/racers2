#ifndef LR2_GLOBALOVERLAYGLYPH_H
#define LR2_GLOBALOVERLAYGLYPH_H

#include "bitmapfont.h"
#include "types.h"

#include <stddef.h>

struct GlobalOverlayRecord;

struct GlobalOverlayGlyphEntryView {
	LegoU16 m_text[10]; // 0x00, inline prefix used as the replacement key
	LegoFloat m_scaleX; // 0x14
	LegoFloat m_scaleY; // 0x18
	undefined m_opaque0x1c[4];
	LegoU32 m_texture;       // 0x20
	LegoFloat m_u;           // 0x24
	LegoFloat m_v;           // 0x28
	LegoFloat m_uEnd;        // 0x2c
	LegoFloat m_vEnd;        // 0x30
	LegoFloat m_widthScale;  // 0x34
	LegoFloat m_heightScale; // 0x38
};

struct GlobalOverlayGlyphReplacementManagerView {
	undefined m_opaque0x00[4];
	GlobalOverlayGlyphEntryView* m_entries[20]; // 0x04
	LegoS32 m_entryCount;                       // 0x54
};

DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayGlyphEntryView, m_scaleX) == 0x14);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayGlyphEntryView, m_texture) == 0x20);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayGlyphEntryView, m_widthScale) == 0x34);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayGlyphReplacementManagerView, m_entries) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(GlobalOverlayGlyphReplacementManagerView, m_entryCount) == 0x54);

LegoBool32 __fastcall HasGlobalOverlayGlyphReplacement(
	GlobalOverlayGlyphReplacementManagerView* p_manager,
	void*,
	const LegoU16* p_text,
	LegoU32* p_matchedLength
);
void __fastcall ApplyGlobalOverlayGlyphReplacement(
	GlobalOverlayGlyphReplacementManagerView* p_manager,
	void*,
	LegoU32 p_glyphIndex,
	GlobalOverlayRecord* p_record,
	BitmapGlyphQuad* p_quad,
	LegoFloat* p_advance
);
LegoFloat __fastcall MeasureGlobalOverlayGlyphReplacement(
	GlobalOverlayGlyphReplacementManagerView* p_manager,
	void*,
	const LegoU16* p_text,
	LegoU32* p_matchedLength,
	BitmapFont* p_font
);
LegoFloat __fastcall GetGlobalOverlayGlyphReplacementAdvance(
	GlobalOverlayGlyphReplacementManagerView* p_manager,
	void*,
	LegoU32 p_glyphIndex,
	BitmapFont* p_font
);

#endif // LR2_GLOBALOVERLAYGLYPH_H
