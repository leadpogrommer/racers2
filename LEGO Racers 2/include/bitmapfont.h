#ifndef LR2_BITMAPFONT_H
#define LR2_BITMAPFONT_H

#include "decomp.h"
#include "types.h"

struct BitmapFontGlyph {
	LegoU32 m_texture;   // 0x00
	LegoFloat m_u;       // 0x04
	LegoFloat m_v;       // 0x08
	LegoFloat m_width;   // 0x0c
	LegoFloat m_height;  // 0x10
	LegoFloat m_uExtent; // 0x14
	LegoFloat m_vExtent; // 0x18
};

DECOMP_SIZE_ASSERT(BitmapFontGlyph, 0x1c);

struct BitmapGlyphQuad {
	LegoFloat m_left;   // 0x00
	LegoFloat m_top;    // 0x04
	LegoFloat m_right;  // 0x08
	LegoFloat m_bottom; // 0x0c
	LegoU8 m_unk0x10[0x2c - 0x10];
	LegoU32 m_texture; // 0x2c
	LegoFloat m_u;     // 0x30
	LegoFloat m_v;     // 0x34
	LegoFloat m_uEnd;  // 0x38
	LegoFloat m_vEnd;  // 0x3c
};

class BitmapFont {
public:
	LegoBool32 GetGlyphMetrics(LegoU16 p_character, BitmapGlyphQuad* p_quad, LegoFloat* p_advance);
	LegoBool32 GetTextAdvance(const LegoU16* p_text, LegoU32* p_matchedLength, LegoFloat* p_advance);
	LegoBool32 Load(
		LegoU32 p_fontIndex,
		const LegoChar* p_path,
		const LegoChar* p_characterMap,
		LegoS32 p_glyphWidth,
		LegoU32 p_caseMode
	);
	void CopyFrom(LegoU32 p_resourceType, const BitmapFont* p_source);

	LegoU32 m_fontIndex;    // 0x00
	LegoU32 m_bitsPerPixel; // 0x04
	LegoS32 m_glyphCount;   // 0x08
	LegoU32 m_unk0x0c;      // 0x0c
	LegoS32 m_unk0x10;
	LegoFloat m_scaleX;            // 0x14
	LegoFloat m_scaleY;            // 0x18
	LegoFloat m_unk0x1c;           // 0x1c
	LegoFloat m_unk0x20;           // 0x20
	LegoFloat m_advanceWidth;      // 0x24
	LegoFloat m_unk0x28;           // 0x28
	LegoFloat m_spacing;           // 0x2c
	BitmapFontGlyph m_glyphs[256]; // 0x30
	LegoU8 m_characterMap[256];    // 0x1c30
	LegoU32 m_textureIds[8];       // 0x1d30
	LegoS32 m_textureCount;        // 0x1d50

	// The font allocation size and texture list are established by its callers.
};

DECOMP_SIZE_ASSERT(BitmapFont, 0x1d54);

#endif // LR2_BITMAPFONT_H
