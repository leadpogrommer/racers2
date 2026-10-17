#include "bitmaputil.h"

// FUNCTION: LEGORACERS2 0x004c3460
LegoS32 FindFirstNonemptyRow(
	LegoS32 p_bitsPerPixel,
	const LegoU8* p_pixels,
	LegoS32 p_rowStride,
	LegoS32 p_rows,
	LegoS32 p_pixelStride,
	LegoS32 p_columns
)
{
	LegoS32 row = 0;
	switch (p_bitsPerPixel) {
	case 8:
		while (p_rows-- > 0) {
			const LegoU8* pixel = p_pixels;
			LegoS32 columns = p_columns;
			while (columns-- > 0) {
				if (*pixel != 0xff) {
					return row;
				}
				pixel += p_pixelStride;
			}
			p_pixels += p_rowStride;
			++row;
		}
		break;
	case 32:
		p_rowStride *= 4;
		while (p_rows-- > 0) {
			const LegoU8* pixel = p_pixels;
			LegoS32 columns = p_columns;
			while (columns-- > 0) {
				if (pixel[3] != 0) {
					return row;
				}
				pixel += p_pixelStride * 4;
			}
			p_pixels += p_rowStride;
			++row;
		}
		break;
	}
	return -1;
}
