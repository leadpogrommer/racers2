#ifndef LR2_BITMAPUTIL_H
#define LR2_BITMAPUTIL_H

#include "types.h"

LegoS32 FindFirstNonemptyRow(
	LegoS32 p_bitsPerPixel,
	const LegoU8* p_pixels,
	LegoS32 p_rowStride,
	LegoS32 p_rows,
	LegoS32 p_pixelStride,
	LegoS32 p_columns
);

#endif // LR2_BITMAPUTIL_H
