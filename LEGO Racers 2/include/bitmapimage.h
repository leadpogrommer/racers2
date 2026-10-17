#ifndef LR2_BITMAPIMAGE_H
#define LR2_BITMAPIMAGE_H

#include "types.h"

class BitmapImage {
public:
	BitmapImage();

	void* m_palette;              // 0x00
	LegoU8* m_pixels;             // 0x04
	LegoS16 m_width;              // 0x08
	LegoS16 m_height;             // 0x0a
	LegoU8 m_bitsPerPixel;        // 0x0c
	LegoU8 m_mipLevels;           // 0x0d
	LegoU16 m_paletteEntries : 9; // 0x0e, bits 0-8
	LegoU16 m_unk0x0fBit1 : 1;
	LegoU16 m_unk0x0fBit2 : 1;
	LegoU16 m_unk0x0fBits3To7 : 5;
	LegoS32 m_unk0x10; // 0x10

	// Partial image model; the constructor leaves the upper five flag bits untouched.
};

#endif // LR2_BITMAPIMAGE_H
