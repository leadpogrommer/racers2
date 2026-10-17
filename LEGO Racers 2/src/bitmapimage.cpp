#include "bitmapimage.h"

// FUNCTION: LEGORACERS2 0x004c3d70
BitmapImage::BitmapImage()
	: m_palette(NULL), m_pixels(NULL), m_width(0), m_height(0), m_bitsPerPixel(0), m_mipLevels(1),
	  m_paletteEntries(256), m_unk0x0fBit1(0), m_unk0x0fBit2(1), m_unk0x10(-1)
{
}
