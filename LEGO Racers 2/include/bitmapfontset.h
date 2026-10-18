#ifndef LR2_BITMAPFONTSET_H
#define LR2_BITMAPFONTSET_H

#include "bitmapfont.h"

class BitmapFontSet {
public:
	BitmapFontSet();
	~BitmapFontSet();
	void Initialize(LegoU32 p_mode);
	void Release(LegoU32 p_mode);
	void ReleaseAll();

	BitmapFont* m_fonts[4];
};

DECOMP_SIZE_ASSERT(BitmapFontSet, 0x10);

#endif // LR2_BITMAPFONTSET_H
