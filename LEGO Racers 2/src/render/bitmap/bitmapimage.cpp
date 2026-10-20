#include "render/bitmap/bitmapimage.h"

#include "core/application/application.h"
#include "core/debug/diagnostics.h"
#include "io/files/winfile.h"
#include "memory/memoryheap.h"

namespace
{
LegoS32 GetMipChainByteCount(LegoS16 p_width, LegoS16 p_height, LegoU8 p_mipLevels, LegoU32 p_bytesPerPixel)
{
	LegoS32 byteCount = 0;
	for (LegoS32 level = 0; level < (LegoS32) p_mipLevels; level++) {
		LegoS32 shift = level & 0x1f;
		byteCount += ((LegoS32) p_width >> shift) * ((LegoS32) p_height >> shift) * (LegoS32) p_bytesPerPixel;
	}
	return byteCount;
}
} // namespace

// FUNCTION: LEGORACERS2 0x004c3d70
BitmapImage::BitmapImage()
	: m_palette(NULL), m_pixels(NULL), m_width(0), m_height(0), m_bitsPerPixel(0), m_mipLevels(1),
	  m_paletteEntries(256), m_unk0x0fBit1(0), m_unk0x0fBit2(1), m_unk0x10(-1)
{
}

// STUB: LEGORACERS2 0x004f54a0
LegoBool32 BitmapImage::LoadTga(const LegoChar* p_path, LegoU32 p_ignored)
{
	(void) p_ignored;
	LegoU8 header[18];
	LegoU32 defaultMetadata[5];
	LegoU32 imageMetadata[64];
	BOOL opened = FALSE;
	WinFile file(p_path, 0, &opened);
	if (!opened) {
		return FALSE;
	}

	LegoS32 bytesRemaining = (LegoS32) file.GetManagedSize();
	file.Read(header, sizeof(header), 1);
	defaultMetadata[0] = 0x31464941;
	defaultMetadata[1] = 1;
	defaultMetadata[2] = 0;
	defaultMetadata[3] = 1;
	defaultMetadata[4] = 0;

	LegoU8 imageIdLength = header[0];
	const LegoU8 imageType = header[2];
	const LegoU8 pixelDepth = header[16];
	const LegoU8 colorMapDepth = header[7];
	const LegoU16 width = (LegoU16) (header[12] | ((LegoU16) header[13] << 8));
	const LegoU16 height = (LegoU16) (header[14] | ((LegoU16) header[15] << 8));
	const LegoU16 colorMapLength = (LegoU16) (header[5] | ((LegoU16) (header[6] & 1) << 8));

	if (GetApplication()->IsDevelopmentMode()) {
		if (imageType != 1 && imageType != 2 && imageType != 3) {
			ReportFatalError("%s is not an uncompressed palette TGA file", p_path);
		}
		if (pixelDepth != 8 && pixelDepth != 16 && pixelDepth != 24 && pixelDepth != 32) {
			ReportFatalError("%s has %d bits per pixel, which is unsupported", p_path, pixelDepth);
		}
	}

	m_bitsPerPixel = pixelDepth;
	m_height = (LegoS16) height;
	m_width = (LegoS16) width;
	LegoU16 originalFlags = *(LegoU16*) ((LegoU8*) this + 0x0e);
	*(LegoU16*) ((LegoU8*) this + 0x0e) = (LegoU16) (colorMapLength ^ (originalFlags & 0xfe00));

	if (GetApplication()->IsDevelopmentMode() && imageIdLength > 0x100) {
		ReportFatalError("ID field too large in TGA");
	}

	file.Read(imageMetadata, imageIdLength, 1);
	bytesRemaining -= 0x12 + imageIdLength;
	LegoU32* metadata = defaultMetadata;
	if (imageIdLength == 20 && imageMetadata[0] == 0x31464941) {
		metadata = imageMetadata;
	}

	const LegoU16 metadataFlags = (LegoU16) (((metadata[4] & 1) << 1 | (metadata[3] & 1)) << 1 | (metadata[2] & 1))
								  << 9;
	LegoU16 flags = (LegoU16) (metadataFlags | (originalFlags & 0xf1ff));
	*(LegoU16*) ((LegoU8*) this + 0x0e) = flags;
	m_mipLevels = (LegoU8) metadata[1];

	if (pixelDepth == 8) {
		flags = (LegoU16) ((metadataFlags | (originalFlags & 0xf000)) ^ 0x100);
		*(LegoU16*) ((LegoU8*) this + 0x0e) = flags;
		m_unk0x10 = 2;
		m_palette = GetMemoryHeap()->AllocateFromEnd(0x400, "Palette Data TGA: LoadFile");

		const LegoS32 pixelBytes = GetMipChainByteCount(m_width, m_height, m_mipLevels, pixelDepth >> 3);
		m_pixels = (LegoU8*) GetMemoryHeap()->AllocateFromEnd((LegoU32) pixelBytes, "BitMap Data TGA: LoadFile");
		LegoS32 paletteBytes = (LegoS32) (flags & 0x1ff) * (colorMapDepth >> 3);
		file.Read(m_palette, paletteBytes, 1);
		bytesRemaining -= paletteBytes;

		if (colorMapDepth == 24) {
			LegoU8* source = (LegoU8*) m_palette + ((LegoU32) (colorMapDepth >> 3) * 0xff);
			LegoU8* destination = (LegoU8*) m_palette + 0x3fc;
			for (LegoS32 entry = 0x100; entry != 0; entry--) {
				destination[3] = 0;
				destination[2] = source[2];
				destination[1] = source[1];
				destination[0] = source[0];
				source -= colorMapDepth >> 3;
				destination -= 4;
			}
			for (LegoS32 alphaOffset = 3; alphaOffset < 0x400; alphaOffset += 4) {
				((LegoU8*) m_palette)[alphaOffset] = 0xff;
			}
			if (*(LegoU32*) ((LegoU8*) m_palette + 0x3fc) == 0xff000000) {
				((LegoU8*) m_palette)[0x3ff] = 0;
			}
		}
		else if (colorMapDepth != 32 && GetApplication()->IsDevelopmentMode() && imageType != 3) {
			ReportFatalError("%s has %d bits per colour map entry", p_path, colorMapDepth);
		}

		LegoS32 expectedPixelBytes = GetMipChainByteCount(m_width, m_height, m_mipLevels, pixelDepth >> 3);
		LegoS32 readPixelBytes = expectedPixelBytes;
		if (bytesRemaining < expectedPixelBytes) {
			readPixelBytes = bytesRemaining;
		}
		else {
			bytesRemaining = 0;
		}
		file.Read(m_pixels, (DWORD) readPixelBytes, 1);
	}
	else {
		flags = (LegoU16) (metadataFlags | (originalFlags & 0xf000));
		*(LegoU16*) ((LegoU8*) this + 0x0e) = flags;
		m_unk0x10 = 1;
		m_palette = NULL;
		if (pixelDepth == 32) {
			m_unk0x0fBits3To7 = (LegoU16) (m_unk0x0fBits3To7 | 1);
		}
		const LegoS32 pixelBytes = GetMipChainByteCount(m_width, m_height, m_mipLevels, 4);
		m_pixels = (LegoU8*) GetMemoryHeap()->AllocateFromEnd((LegoU32) pixelBytes, "BitMap Data TGA: LoadFile");
		const LegoS32 sourcePixelBytes = GetMipChainByteCount(m_width, m_height, m_mipLevels, pixelDepth >> 3);
		LegoS32 readPixelBytes = sourcePixelBytes;
		if (bytesRemaining < sourcePixelBytes) {
			readPixelBytes = bytesRemaining;
		}
		file.Read(m_pixels, (DWORD) readPixelBytes, 1);
	}

	return TRUE;
}

// FUNCTION: LEGORACERS2 0x004f59c0
void BitmapImage::ReleaseStorage()
{
	if (m_pixels != NULL) {
		GetMemoryHeap()->Free(m_pixels);
		m_pixels = NULL;
	}
	if (m_palette != NULL) {
		GetMemoryHeap()->Free(m_palette);
		m_palette = NULL;
	}
}
