#ifndef LR2_PIXELFORMAT_H
#define LR2_PIXELFORMAT_H

#include "decomp.h"
#include "directx8.h"
#include "types.h"

#include <stddef.h>

struct PixelFormatDescription {
	void Initialize(D3DFORMAT p_format);

	LegoU8 m_redLoss;        // 0x00, eight minus channel width for supported masks
	LegoU8 m_redShift;       // 0x01
	LegoU8 m_greenLoss;      // 0x02
	LegoU8 m_greenShift;     // 0x03
	LegoU8 m_blueLoss;       // 0x04
	LegoU8 m_blueShift;      // 0x05
	LegoU8 m_alphaLoss;      // 0x06
	LegoU8 m_alphaShift;     // 0x07
	LegoU32 m_redMask;       // 0x08
	LegoU32 m_greenMask;     // 0x0c
	LegoU32 m_blueMask;      // 0x10
	LegoU32 m_alphaMask;     // 0x14
	LegoS32 m_bitsPerPixel;  // 0x18
	LegoS32 m_bytesPerPixel; // 0x1c
};

DECOMP_SIZE_ASSERT(PixelFormatDescription, 0x20);
DECOMP_STATIC_ASSERT(offsetof(PixelFormatDescription, m_redMask) == 8);
DECOMP_STATIC_ASSERT(offsetof(PixelFormatDescription, m_bitsPerPixel) == 0x18);

LegoU32 WINAPI GetChannelMaskBitCount(LegoU32 p_mask);

#endif // LR2_PIXELFORMAT_H
