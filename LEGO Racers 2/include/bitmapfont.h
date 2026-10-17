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

	LegoU8 m_unk0x00[0x14];
	LegoFloat m_scaleX; // 0x14
	LegoFloat m_scaleY; // 0x18
	LegoU8 m_unk0x1c[0x2c - 0x1c];
	LegoFloat m_spacing;           // 0x2c
	BitmapFontGlyph m_glyphs[256]; // 0x30
	LegoU8 m_characterMap[256];    // 0x1c30

	// The font and output quad layouts are partial; their full extents are unknown.
};

#endif // LR2_BITMAPFONT_H
