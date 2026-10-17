#include "bitmapimage.h"
#include "memoryheap.h"
#include "renderer.h"
#include "rendererrecords.h"
#include "winfile.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

LegoU8 __stdcall FindClosestPaletteColor(const LegoU8* p_palette, LegoU32 p_color);
LegoBool32 __stdcall ParseBitmapInfoValue(const LegoChar* p_text, const LegoChar* p_key, LegoU32* p_value);
LegoBool32 __stdcall GenerateBitmapMipLevel(LegoS32 p_level, BitmapImage* p_image);
LegoBool32 __fastcall WriteBitmapTgaFile(BitmapImage* p_image, void*, const LegoChar* p_path);
void __fastcall ResampleBitmapImage(
	DisplayRenderer* p_renderer,
	void*,
	const BitmapImage* p_source,
	BitmapImage* p_destination
);
void __fastcall UpdateTextureRecordFormat(
	RendererTextureRecord* p_record,
	void*,
	BitmapImage* p_image,
	LegoU32 p_deviceMemory
);
void __fastcall ReleaseRendererTextureResource(RendererTextureRecord* p_record, void*);

namespace
{
const LegoFloat g_normalMapPaletteScale = 0.0013071897f;
const LegoFloat g_normalMapAlphaScale = 0.0039215689f;
const LegoFloat g_normalMapGradientScale = 10.0f;
const LegoFloat g_normalMapUnitLength = 1.0f;
const LegoFloat g_normalMapChannelScale = 127.0f;
const LegoFloat g_normalMapChannelCenter = 128.0f;

void UpdateTextureRecordFormatImpl(RendererTextureRecord* p_record, BitmapImage* p_image, LegoU32 p_deviceMemory);
void ResampleBitmapImageImpl(const BitmapImage* p_source, BitmapImage* p_destination);
LegoBool32 LockTextureSurface(
	DisplayRenderer* p_renderer,
	void*,
	IDirect3DSurface8* p_surface,
	D3DLOCKED_RECT* p_locked
);
void UnlockTextureSurface(DisplayRenderer* p_renderer, void*, IDirect3DSurface8* p_surface);

LegoS32 GetMipPixelCount(const BitmapImage* p_image)
{
	LegoS32 pixels = 0;
	for (LegoS32 level = 0; level < p_image->m_mipLevels; level++) {
		const LegoU32 shift = (LegoU32) level & 0x1f;
		pixels += ((LegoS32) p_image->m_width >> shift) * ((LegoS32) p_image->m_height >> shift);
	}
	return pixels;
}

LegoS32 GetMipByteCount(const BitmapImage* p_image, LegoU32 p_bytesPerPixel)
{
	LegoS32 bytes = 0;
	for (LegoS32 level = 0; level < p_image->m_mipLevels; level++) {
		const LegoU32 shift = (LegoU32) level & 0x1f;
		bytes +=
			((LegoS32) p_image->m_width >> shift) * ((LegoS32) p_image->m_height >> shift) * (LegoS32) p_bytesPerPixel;
	}
	return bytes;
}

struct BitmapInfoSettings {
	LegoU32 m_magic;
	LegoS32 m_mipLimit;
	LegoU32 m_recommendCompression;
	LegoU32 m_allowCompression;
	LegoU32 m_alphaChannel;
};

LegoBool32 ReadBitmapInfoFile(const LegoChar* p_sourcePath, BitmapInfoSettings* p_settings)
{
	LegoChar path[260];
	strcpy(path, p_sourcePath);
	LegoS32 length = (LegoS32) strlen(path);
	while (--length >= 0 && path[length] != '.') {
	}
	if (length <= 0) {
		return FALSE;
	}
	strcpy(path + length, ".BIF");
	if (ManagedPathExists(path) != TRUE) {
		LegoS32 slash = (LegoS32) strlen(path);
		while (--slash >= 0 && path[slash] != '\\') {
		}
		strcpy(path + slash + 1, "ALL_FILES.BIF");
	}
	if (ManagedPathExists(path) != TRUE) {
		return FALSE;
	}

	BOOL opened = FALSE;
	WinFile file(path, 0, &opened);
	if (opened != 1) {
		return FALSE;
	}
	const DWORD size = file.GetManagedSize();
	LegoU8* text = (LegoU8*) GetMemoryHeap()->AllocateFromEnd(size + 1, "Temporary bitmap info");
	file.Read(text, size, 0);
	text[size] = 0;
	_strupr((LegoChar*) text);
	LegoU32 value;
	if (ParseBitmapInfoValue((LegoChar*) text, "MIP MAPS", &value)) {
		p_settings->m_mipLimit = (LegoS32) value;
	}
	if (ParseBitmapInfoValue((LegoChar*) text, "RECOMMEND COMPRESSION", &value)) {
		p_settings->m_recommendCompression = value;
	}
	if (ParseBitmapInfoValue((LegoChar*) text, "ALLOW COMPRESSION", &value)) {
		p_settings->m_allowCompression = value;
	}
	if (ParseBitmapInfoValue((LegoChar*) text, "ALPHA CHANNEL", &value)) {
		p_settings->m_alphaChannel = value;
	}
	GetMemoryHeap()->Free(text);
	return TRUE;
}

void PrepareBitmapImageForTexture(BitmapImage* p_image)
{
	if (p_image->m_bitsPerPixel == 24) {
		const LegoS32 pixelCount = GetMipPixelCount(p_image);
		LegoU8* source = p_image->m_pixels + pixelCount * 3 - 3;
		LegoU32* destination = (LegoU32*) (p_image->m_pixels + pixelCount * 4 - 4);
		for (LegoS32 i = pixelCount; i > 0; i--) {
			LegoU32 pixel = *(LegoU32*) source;
			*destination = pixel | 0xff000000;
			source -= 3;
			destination--;
		}
	}
	else if (p_image->m_bitsPerPixel == 16) {
		const LegoS32 pixelCount = GetMipPixelCount(p_image);
		LegoS16* source = (LegoS16*) (p_image->m_pixels + pixelCount * 2 - 2);
		LegoU32* destination = (LegoU32*) (p_image->m_pixels + pixelCount * 4 - 4);
		for (LegoS32 i = pixelCount; i > 0; i--) {
			const LegoS32 pixel = *source;
			*destination = ((LegoU32) (pixel << 9) & 0x00f80000) | ((LegoU32) (pixel << 6) & 0x0000f800) |
						   ((LegoU32) (pixel << 3) & 0x000000f8) | ((LegoU32) ~pixel & 0xff000000);
			source--;
			destination--;
		}
	}

	if (p_image->m_bitsPerPixel != 8) {
		p_image->m_bitsPerPixel = 32;
	}
}
} // namespace

// STUB: LEGORACERS2 0x004d8950
void __fastcall ResampleBitmapImage(DisplayRenderer*, void*, const BitmapImage* p_source, BitmapImage* p_destination)
{
	ResampleBitmapImageImpl(p_source, p_destination);
}

// STUB: LEGORACERS2 0x004d92a0
LegoBool32 __fastcall UploadBitmapMipChain(
	DisplayRenderer* p_renderer,
	void*,
	RendererTextureRecord* p_record,
	BitmapImage* p_image,
	LegoU32 p_cubeFace
)
{
	BitmapImage source;
	memcpy(&source, p_image, sizeof(source));
	const LegoU32 resolutionOffset = (p_record->m_flags0x9c & 0x20) != 0 ? 1 : 0;
	LegoU32 sourceLevel = resolutionOffset;
	const LegoU32 sourceMipCount = source.m_mipLevels;
	if ((LegoS32) p_record->m_width != ((LegoS32) source.m_width >> sourceLevel) ||
		(LegoS32) p_record->m_height != ((LegoS32) source.m_height >> sourceLevel)) {
		LegoU32 mipOffset;
		for (mipOffset = 0; mipOffset < sourceMipCount; mipOffset++) {
			const LegoU32 shift = (resolutionOffset + mipOffset) & 0x1f;
			if ((LegoS32) p_record->m_width == ((LegoS32) source.m_width >> shift) &&
				(LegoS32) p_record->m_height == ((LegoS32) source.m_height >> shift)) {
				sourceLevel += mipOffset;
				break;
			}
		}
		if (mipOffset == sourceMipCount) {
			// A failed search leaves the original resolution-mode level intact.
			source.m_width = (LegoS16) ((LegoS32) p_record->m_width << sourceLevel);
			source.m_height = (LegoS16) ((LegoS32) p_record->m_height << sourceLevel);
			LegoS16 smaller = source.m_width < source.m_height ? source.m_width : source.m_height;
			LegoU32 maximumLevels = 0;
			while (smaller > 0) {
				smaller >>= 1;
				maximumLevels++;
			}
			if ((maximumLevels & 0xff) < source.m_mipLevels) {
				source.m_mipLevels = (LegoU8) maximumLevels;
			}
			const LegoU32 bytes = (LegoU32) GetMipByteCount(&source, source.m_bitsPerPixel >> 3);
			source.m_pixels = (LegoU8*) GetMemoryHeap()->AllocateFromEnd(bytes, "Temporary scaled bitmap");
			ResampleBitmapImage(p_renderer, NULL, p_image, &source);
		}
	}

	const LegoU32 encodedMipCount = p_record->m_flags0x90 >> 9;
	if ((p_record->m_flags0x9c & 8) != 0) {
		const LegoU32 face = p_cubeFace <= 5 ? p_cubeFace : 0;
		for (LegoU32 level = 0; level < encodedMipCount; level++) {
			IDirect3DSurface8* surface;
			((IDirect3DCubeTexture8*) p_record->m_primaryTexture)
				->GetCubeMapSurface((D3DCUBEMAP_FACES) face, level, &surface);
			D3DLOCKED_RECT locked;
			if (LockTextureSurface(p_renderer, NULL, surface, &locked)) {
				p_renderer->CopyBitmapMipToLockedSurface(
					sourceLevel + level,
					&source,
					p_renderer->m_selectedTextureFormats[(p_record->m_flags0x9c >> 6) & 0xffff],
					&locked
				);
				UnlockTextureSurface(p_renderer, NULL, surface);
			}
			surface->Release();
		}
	}
	else {
		for (LegoU32 level = 0; level < encodedMipCount; level++) {
			IDirect3DSurface8* surface;
			((IDirect3DTexture8*) p_record->m_primaryTexture)->GetSurfaceLevel(level, &surface);
			D3DLOCKED_RECT locked;
			if (LockTextureSurface(p_renderer, NULL, surface, &locked)) {
				p_renderer->CopyBitmapMipToLockedSurface(
					sourceLevel + level,
					&source,
					p_renderer->m_selectedTextureFormats[(p_record->m_flags0x9c >> 6) & 0xffff],
					&locked
				);
				UnlockTextureSurface(p_renderer, NULL, surface);
			}
			surface->Release();
		}
	}
	if (source.m_pixels != p_image->m_pixels) {
		GetMemoryHeap()->Free(source.m_pixels);
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004d9b70
void DisplayRenderer::PrepareBitmapForTexture(BitmapImage* p_image)
{
	PrepareBitmapImageForTexture(p_image);
}

// STUB: LEGORACERS2 0x004d9cb0
LegoBool32 DisplayRenderer::LoadTextureImage(
	LegoS32 p_id,
	BitmapImage* p_image,
	LegoS32 p_resourceType,
	const LegoChar* p_sourcePath
)
{
	SetTextureIndexMap((LegoU32) p_id, (LegoU32) p_id);
	RendererTextureRecord* record = &m_textureRecords[p_id];
	if (!SetTextureRecordDimensions(
			record,
			p_image->m_width,
			p_image->m_height,
			p_image->m_mipLevels,
			0,
			p_image->m_bitsPerPixel,
			0
		)) {
		return FALSE;
	}
	UpdateTextureRecordFormat(record, NULL, p_image, m_totalPhysicalMemory);
	LegoBool32 compressed = FALSE;
	if (m_vtable->m_textureQuality(this, NULL) < 5 && m_unk0x4d8 < 6) {
		compressed = (*((LegoU16*) ((LegoU8*) p_image + 0x0e)) >> 9) & 1;
	}
	else {
		compressed = (*((LegoU16*) ((LegoU8*) p_image + 0x0e)) >> 10) & 1;
	}
	if (m_unk0x4d8 == 1 && m_vtable->m_textureQuality(this, NULL) < 2) {
		compressed = FALSE;
	}
	if (!CreatePrimaryTexture(record, compressed)) {
		ReleaseRendererTextureResource(record, NULL);
		record->m_flags0x06 &= 0xfffe;
		return FALSE;
	}
	if (m_createDeviceBuffers == 1) {
		CreateSecondaryTexture(record);
	}
	PrepareBitmapForTexture(p_image);
	if (!UploadBitmapMipChain(this, NULL, record, p_image, 0)) {
		return FALSE;
	}
	record->UpdateGpuTexture();
	return TRUE;
}

// STUB: LEGORACERS2 0x004d9dc0
LegoBool32 DisplayRenderer::LoadCubeTextureFace(
	LegoS32 p_id,
	BitmapImage* p_image,
	const LegoChar* p_sourcePath,
	LegoS32 p_faceIndex
)
{
	SetTextureIndexMap((LegoU32) p_id, (LegoU32) p_id);
	RendererTextureRecord* record = &m_textureRecords[p_id];
	if (p_faceIndex == 0) {
		if (!SetTextureRecordDimensions(
				record,
				p_image->m_width,
				p_image->m_height,
				p_image->m_mipLevels,
				0,
				p_image->m_bitsPerPixel,
				1
			)) {
			return FALSE;
		}
		if (record->m_width > record->m_height) {
			record->m_height = record->m_width;
		}
		if (record->m_width < record->m_height) {
			record->m_width = record->m_height;
		}
		UpdateTextureRecordFormat(record, NULL, p_image, m_totalPhysicalMemory);
		LegoBool32 compressed = FALSE;
		if (m_vtable->m_textureQuality(this, NULL) < 5 && m_unk0x4d8 < 6) {
			compressed = (*((LegoU16*) ((LegoU8*) p_image + 0x0e)) >> 9) & 1;
		}
		else {
			compressed = (*((LegoU16*) ((LegoU8*) p_image + 0x0e)) >> 10) & 1;
		}
		if (m_unk0x4d8 == 1 && m_vtable->m_textureQuality(this, NULL) < 2) {
			compressed = FALSE;
		}
		if (!CreatePrimaryTexture(record, compressed)) {
			ReleaseRendererTextureResource(record, NULL);
			record->m_flags0x06 &= 0xfffe;
			return FALSE;
		}
		if (m_createDeviceBuffers == 1) {
			CreateSecondaryTexture(record);
		}
	}
	PrepareBitmapForTexture(p_image);
	if (!UploadBitmapMipChain(this, NULL, record, p_image, (LegoU32) p_faceIndex)) {
		return FALSE;
	}
	record->UpdateGpuTexture();
	return TRUE;
}

// STUB: LEGORACERS2 0x004da5e0
LegoBool32 DisplayRenderer::BuildMipSidecar(
	const LegoChar* p_sourcePath,
	const LegoChar* p_outputSuffix,
	BitmapImage* p_sourceImage
)
{
	BitmapImage image;
	LegoChar outputPath[264];
	strcpy(outputPath, p_sourcePath);
	LegoS32 pathLength = (LegoS32) strlen(outputPath);
	LegoS32 extension = pathLength;
	while (--extension >= 0 && outputPath[extension] != '.') {
	}
	if (extension >= 0) {
		strcpy(outputPath + extension, p_outputSuffix);
		_strupr(outputPath);
	}

	BitmapInfoSettings settings;
	settings.m_magic = 0x31464941;
	settings.m_mipLimit = 100;
	settings.m_recommendCompression = 1;
	settings.m_allowCompression = 1;
	settings.m_alphaChannel = 1;
	ReadBitmapInfoFile(p_sourcePath, &settings);

	memcpy(&image, p_sourceImage, sizeof(image));
	LegoS32 minimumDimension = image.m_width < image.m_height ? image.m_width : image.m_height;
	LegoU32 mipLevels = 0;
	while (minimumDimension > 0) {
		minimumDimension >>= 1;
		mipLevels++;
	}
	if (strstr(p_sourcePath, "_PACKED") != NULL && settings.m_mipLimit == 100) {
		settings.m_mipLimit = 0;
	}
	if (settings.m_mipLimit + 1 < (LegoS32) mipLevels) {
		mipLevels = settings.m_mipLimit + 1;
	}
	image.m_mipLevels = (LegoU8) mipLevels;
	LegoU16* imageFlags = (LegoU16*) ((LegoU8*) &image + 0x0e);
	*imageFlags = (LegoU16) (((settings.m_alphaChannel & 1) << 11) | ((settings.m_recommendCompression & 1) << 9) |
							 ((settings.m_allowCompression & 1) << 10) | (*imageFlags & 0xf1ff));

	if (p_sourceImage->m_palette == NULL) {
		image.m_palette = NULL;
	}
	else {
		image.m_palette = GetMemoryHeap()->AllocateFromEnd(0x400, "MipMap Palette");
		memcpy(image.m_palette, p_sourceImage->m_palette, 0x400);
	}
	const LegoS32 allocationBytes = GetMipByteCount(&image, image.m_bitsPerPixel >> 3);
	image.m_pixels = (LegoU8*) GetMemoryHeap()->AllocateFromEnd((LegoU32) allocationBytes, "MipMap Bitmap");
	memset(image.m_pixels, 0, allocationBytes);
	memcpy(image.m_pixels, p_sourceImage->m_pixels, GetMipByteCount(p_sourceImage, p_sourceImage->m_bitsPerPixel >> 3));
	if ((imageFlags[0] & 0x800) != 0 && image.m_bitsPerPixel == 8) {
		LegoU8* palette = (LegoU8*) image.m_palette;
		for (LegoS32 i = 0; i < 256; i++) {
			LegoU8 maximum = palette[i * 4];
			if (palette[i * 4 + 1] > maximum) {
				maximum = palette[i * 4 + 1];
			}
			if (palette[i * 4 + 2] > maximum) {
				maximum = palette[i * 4 + 2];
			}
			palette[i * 4 + 3] = maximum;
		}
	}
	for (LegoS32 level = 1; level < (LegoS32) mipLevels; level++) {
		GenerateBitmapMipLevel(level, &image);
	}
	WriteBitmapTgaFile(&image, NULL, outputPath);

	pathLength = (LegoS32) strlen(outputPath);
	extension = pathLength;
	while (--extension >= 0 && outputPath[extension] != '.') {
	}
	strcpy(outputPath + extension, ".NOMIP");
	image.m_mipLevels = 1;
	WriteBitmapTgaFile(&image, NULL, outputPath);
	image.ReleaseStorage();
	return TRUE;
}

// STUB: LEGORACERS2 0x004da9f0
LegoBool32 ReadBitmapInfoSettings(const LegoChar* p_sourcePath, BitmapInfoSettings* p_settings)
{
	return ReadBitmapInfoFile(p_sourcePath, p_settings);
}

// STUB: LEGORACERS2 0x004dabe0
LegoBool32 __stdcall ParseBitmapInfoValue(const LegoChar* p_text, const LegoChar* p_key, LegoU32* p_value)
{
	LegoChar digits[261] = {0};
	const LegoChar* value = strstr(p_text, p_key);
	if (value == NULL) {
		return FALSE;
	}
	value += strlen(p_key);
	while (*value != '\0' && (*value <= '/' || *value >= ':')) {
		value++;
	}
	if (*value == '\0') {
		return FALSE;
	}
	LegoU32 length = 0;
	while (*value > '/' && *value < ':') {
		digits[length++] = *value++;
	}
	digits[length] = '\0';
	*p_value = (LegoU32) atoi(digits);
	return TRUE;
}

// STUB: LEGORACERS2 0x004dac90
LegoBool32 __stdcall GenerateBitmapMipLevel(LegoS32 p_level, BitmapImage* p_image)
{
	const LegoU32 shift = (LegoU32) p_level & 0x1f;
	const LegoS32 destinationWidth = (LegoS32) p_image->m_width >> shift;
	const LegoS32 destinationHeight = (LegoS32) p_image->m_height >> shift;
	const LegoS32 sourceWidth = (LegoS32) p_image->m_width >> ((LegoU32) (p_level - 1) & 0x1f);
	const LegoS32 bytesPerPixel = p_image->m_bitsPerPixel >> 3;

	LegoS32 sourceOffset = 0;
	if (p_image->m_mipLevels >= 2) {
		for (LegoS32 level = 0; level < p_level - 1; level++) {
			const LegoU32 sourceShift = (LegoU32) level & 0x1f;
			sourceOffset += ((LegoS32) p_image->m_width >> sourceShift) * ((LegoS32) p_image->m_height >> sourceShift) *
							bytesPerPixel;
		}
	}
	const LegoU8* source = p_image->m_pixels + sourceOffset;

	LegoS32 destinationOffset = 0;
	for (LegoS32 level = 0; level < p_level; level++) {
		const LegoU32 destinationShift = (LegoU32) level & 0x1f;
		destinationOffset += ((LegoS32) p_image->m_width >> destinationShift) *
							 ((LegoS32) p_image->m_height >> destinationShift) * bytesPerPixel;
	}
	LegoU8* destination = p_image->m_pixels + destinationOffset;

	if (p_image->m_bitsPerPixel == 8) {
		for (LegoS32 y = 0; y < destinationHeight; y++) {
			for (LegoS32 x = 0; x < destinationWidth; x++) {
				const LegoS32 sourceIndex = (y * 2) * sourceWidth + x * 2;
				const LegoU8 sourceIndices[4] = {
					source[sourceIndex],
					source[sourceIndex + 1],
					source[sourceIndex + sourceWidth],
					source[sourceIndex + sourceWidth + 1]
				};
				LegoU32 channels[4] = {0, 0, 0, 0};
				const LegoU8* palette = (const LegoU8*) p_image->m_palette;
				for (LegoS32 sample = 0; sample < 4; sample++) {
					const LegoU8* color = palette + sourceIndices[sample] * 4;
					for (LegoS32 channel = 0; channel < 4; channel++) {
						channels[channel] += color[channel];
					}
				}
				const LegoU32 average = (channels[0] >> 2) | ((channels[1] >> 2) << 8) | ((channels[2] >> 2) << 16) |
										((channels[3] >> 2) << 24);
				destination[y * destinationWidth + x] = FindClosestPaletteColor(palette, average);
			}
		}
	}
	else if (p_image->m_bitsPerPixel == 16) {
		const LegoU16* input = (const LegoU16*) source;
		LegoU16* output = (LegoU16*) destination;
		for (LegoS32 y = 0; y < destinationHeight; y++) {
			for (LegoS32 x = 0; x < destinationWidth; x++) {
				const LegoS32 index = (y * 2) * sourceWidth + x * 2;
				const LegoU16 a = input[index];
				const LegoU16 b = input[index + 1];
				const LegoU16 c = input[index + sourceWidth];
				const LegoU16 d = input[index + sourceWidth + 1];
				const LegoU32 red = ((a >> 10 & 0x1f) + (b >> 10 & 0x1f) + (c >> 10 & 0x1f) + (d >> 10 & 0x1f)) >> 2;
				const LegoU32 alpha = ((a >> 15) + (b >> 15) + (c >> 15) + (d >> 15)) >> 2;
				const LegoU32 green = ((a >> 5 & 0x1f) + (b >> 5 & 0x1f) + (c >> 5 & 0x1f) + (d >> 5 & 0x1f)) >> 2;
				const LegoU32 blue = ((a & 0x1f) + (b & 0x1f) + (c & 0x1f) + (d & 0x1f)) >> 2;
				output[y * destinationWidth + x] = (LegoU16) ((red << 10) | (alpha << 15) | (green << 5) | blue);
			}
		}
	}
	else if (p_image->m_bitsPerPixel == 24 || p_image->m_bitsPerPixel == 32) {
		for (LegoS32 y = 0; y < destinationHeight; y++) {
			for (LegoS32 x = 0; x < destinationWidth; x++) {
				const LegoS32 sourceIndex = ((y * 2) * sourceWidth + x * 2) * bytesPerPixel;
				const LegoS32 destinationIndex = (y * destinationWidth + x) * bytesPerPixel;
				for (LegoS32 channel = 0; channel < bytesPerPixel; channel++) {
					const LegoU32 sum = source[sourceIndex + channel] + source[sourceIndex + bytesPerPixel + channel] +
										source[sourceIndex + sourceWidth * bytesPerPixel + channel] +
										source[sourceIndex + (sourceWidth + 1) * bytesPerPixel + channel];
					destination[destinationIndex + channel] = (LegoU8) (sum >> 2);
				}
			}
		}
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004db5c0
LegoU8 __stdcall FindClosestPaletteColor(const LegoU8* p_palette, LegoU32 p_color)
{
	const LegoU8 alpha = (LegoU8) (p_color >> 24);
	LegoU32 bestDistance = 0xffffffff;
	LegoU8 bestIndex = 0;
	for (LegoU32 i = 0; i < 256; i++) {
		const LegoU8* candidate = p_palette + i * 4;
		if (candidate[3] == alpha) {
			const LegoS32 red = (LegoS32) ((p_color >> 16) & 0xff) - candidate[2];
			const LegoS32 green = (LegoS32) ((p_color >> 8) & 0xff) - candidate[1];
			const LegoS32 blue = (LegoS32) (p_color & 0xff) - candidate[0];
			const LegoU32 distance = (LegoU32) (red * red + green * green + blue * blue);
			if (distance < bestDistance) {
				bestDistance = distance;
				bestIndex = (LegoU8) i;
			}
		}
	}
	return bestIndex;
}

// STUB: LEGORACERS2 0x004db650
LegoBool32 __stdcall GenerateNormalMap(const LegoChar* p_outputPath, BitmapImage* p_sourceImage)
{
	BitmapImage normalMap;
	if (p_sourceImage->m_bitsPerPixel != 8 && p_sourceImage->m_bitsPerPixel != 32) {
		return FALSE;
	}

	const LegoS32 width = p_sourceImage->m_width;
	const LegoS32 height = p_sourceImage->m_height;
	const LegoS32 pixelCount = width * height;
	LegoU8* pixels = (LegoU8*) GetMemoryHeap()->Allocate((LegoU32) (pixelCount * 4), "Temporary normal map");
	memcpy(&normalMap, p_sourceImage, sizeof(normalMap));
	normalMap.m_palette = NULL;
	normalMap.m_pixels = pixels;
	normalMap.m_bitsPerPixel = 32;
	*(LegoU16*) ((LegoU8*) &normalMap + 0x0e) &= 0xfe00;

	const LegoU8* sourcePixels = p_sourceImage->m_pixels;
	const LegoU8* palette = (const LegoU8*) p_sourceImage->m_palette;
	for (LegoS32 y = 0; y < height; y++) {
		for (LegoS32 x = 0; x < width; x++) {
			LegoFloat center;
			LegoFloat right;
			LegoFloat down;
			if (p_sourceImage->m_bitsPerPixel == 8) {
				const LegoU8* pixel = sourcePixels + y * width + x;
				const LegoU32 index = pixel[0];
				const LegoU32 rightIndex = pixel[1]; // Retail crosses to the next row at the right edge.
				const LegoU32 downIndex = sourcePixels[((y + 1) % height) * width + x];
				center =
					(palette[index * 4] + palette[index * 4 + 1] + palette[index * 4 + 2]) * g_normalMapPaletteScale;
				right = (palette[rightIndex * 4] + palette[rightIndex * 4 + 1] + palette[rightIndex * 4 + 2]) *
						g_normalMapPaletteScale;
				down = (palette[downIndex * 4] + palette[downIndex * 4 + 1] + palette[downIndex * 4 + 2]) *
					   g_normalMapPaletteScale;
			}
			else {
				center = sourcePixels[(y * width + x) * 4 + 3] * g_normalMapAlphaScale;
				right = sourcePixels[(y * width + (x + 1) % width) * 4 + 3] * g_normalMapAlphaScale;
				down = sourcePixels[(((y + 1) % height) * width + x) * 4 + 3] * g_normalMapAlphaScale;
			}

			const LegoFloat dx = (center - right) * g_normalMapGradientScale;
			const LegoFloat dy = (center - down) * g_normalMapGradientScale;
			const LegoFloat inverseLength =
				g_normalMapUnitLength / (LegoFloat) sqrt(dx * dx + dy * dy + g_normalMapUnitLength);
			LegoU8* output = pixels + (y * width + x) * 4;
			output[2] = (LegoU8) (-(inverseLength * dx) * g_normalMapChannelScale + g_normalMapChannelCenter);
			output[1] = (LegoU8) ((inverseLength * dy) * g_normalMapChannelScale + g_normalMapChannelCenter);
			output[0] = (LegoU8) (inverseLength * g_normalMapChannelScale + g_normalMapChannelCenter);
			output[3] = 0xff;
		}
	}

	WriteBitmapTgaFile(&normalMap, NULL, p_outputPath);
	normalMap.ReleaseStorage();
	return TRUE;
}

namespace
{
void UpdateTextureRecordFormatImpl(RendererTextureRecord* p_record, BitmapImage* p_image, LegoU32 p_deviceMemory)
{
	LegoU32 flags = p_record->m_flags0x9c;
	switch (p_image->m_bitsPerPixel) {
	case 8:
		flags &= 0xffc0003f;
		break;
	case 16:
		flags = (flags & 0xffc0007f) | 0x40;
		break;
	case 24:
		flags = (flags & 0xffc000bf) | 0x80;
		break;
	default:
		flags = (flags & 0xffc000ff) | 0xc0;
		break;
	}
	if ((LegoS32) p_deviceMemory < 0x06400000 && (flags & 0x003fffc0) > 0x40) {
		flags = (flags & 0xffc0007f) | 0x40;
	}
	if ((((LegoU8*) p_image)[0x0f] & 8) != 0) {
		flags |= 0x100;
	}
	p_record->m_flags0x9c = flags;
}

LegoBool32 LockTextureSurface(DisplayRenderer*, void*, IDirect3DSurface8* p_surface, D3DLOCKED_RECT* p_locked)
{
	return p_surface != NULL && p_surface->LockRect(p_locked, NULL, 0) == D3D_OK ? TRUE : FALSE;
}

void UnlockTextureSurface(DisplayRenderer*, void*, IDirect3DSurface8* p_surface)
{
	p_surface->UnlockRect();
}

void ResampleBitmapImageImpl(const BitmapImage* p_source, BitmapImage* p_destination)
{
	LegoU32 levelCount =
		p_source->m_mipLevels < p_destination->m_mipLevels ? p_source->m_mipLevels : p_destination->m_mipLevels;
	const LegoS32 sourceBytesPerPixel = p_source->m_bitsPerPixel >> 3;
	for (LegoU32 level = 0; level < levelCount; level++) {
		const LegoS32 shift = level & 0x1f;
		const LegoS32 sourceWidth = (LegoS32) p_source->m_width >> shift;
		const LegoS32 sourceHeight = (LegoS32) p_source->m_height >> shift;
		const LegoS32 destinationWidth = (LegoS32) p_destination->m_width >> shift;
		const LegoS32 destinationHeight = (LegoS32) p_destination->m_height >> shift;
		LegoS32 sourceOffset = 0;
		LegoS32 destinationOffset = 0;
		for (LegoU32 previous = 0; previous < level; previous++) {
			const LegoS32 previousShift = previous & 0x1f;
			sourceOffset += ((LegoS32) p_source->m_width >> previousShift) *
							((LegoS32) p_source->m_height >> previousShift) * sourceBytesPerPixel;
			destinationOffset += ((LegoS32) p_destination->m_width >> previousShift) *
								 ((LegoS32) p_destination->m_height >> previousShift) * sourceBytesPerPixel;
		}
		const LegoU8* source = p_source->m_pixels + sourceOffset;
		LegoU8* destination = p_destination->m_pixels + destinationOffset;
		const LegoS32 xStep = (sourceWidth << 16) / destinationWidth;
		const LegoS32 yStep = (sourceHeight << 16) / destinationHeight;
		LegoS32 sourceY = 0;
		for (LegoS32 y = 0; y < destinationHeight; y++) {
			LegoS32 sourceX = 0;
			for (LegoS32 x = 0; x < destinationWidth; x++) {
				const LegoS32 sourceIndex = ((sourceY >> 16) * sourceWidth + (sourceX >> 16)) * sourceBytesPerPixel;
				const LegoS32 destinationIndex = (y * destinationWidth + x) * sourceBytesPerPixel;
				memcpy(destination + destinationIndex, source + sourceIndex, sourceBytesPerPixel);
				sourceX += xStep;
			}
			sourceY += yStep;
		}
	}
}
} // namespace

// STUB: LEGORACERS2 0x004db920
void __fastcall UpdateTextureRecordFormat(
	RendererTextureRecord* p_record,
	void*,
	BitmapImage* p_image,
	LegoU32 p_deviceMemory
)
{
	UpdateTextureRecordFormatImpl(p_record, p_image, p_deviceMemory);
}
// STUB: LEGORACERS2 0x004f5a10
LegoBool32 __fastcall WriteBitmapTgaFile(BitmapImage* p_image, void*, const LegoChar* p_path)
{
	LegoU8 header[18];
	LegoU32 metadata[5];
	BOOL opened = FALSE;
	WinFile file(p_path, 1, &opened);
	if (opened != 1) {
		return FALSE;
	}

	const LegoU32 pathLength = (LegoU32) strlen(p_path);
	LegoBool32 isMip = FALSE;
	if (pathLength > 4 && strcmp(p_path + pathLength - 4, ".MIP") == 0) {
		isMip = TRUE;
	}

	memset(header, 0, sizeof(header));
	header[0] = isMip ? 20 : 0;
	header[1] = p_image->m_bitsPerPixel == 8 ? 1 : 0;
	header[2] = p_image->m_bitsPerPixel == 8 ? 1 : 2;
	*(LegoU16*) (header + 5) = *((LegoU16*) ((LegoU8*) p_image + 0x0e)) & 0x1ff;
	header[7] = isMip ? 32 : 24;
	*(LegoU16*) (header + 12) = (LegoU16) p_image->m_width;
	*(LegoU16*) (header + 14) = (LegoU16) p_image->m_height;
	header[16] = p_image->m_bitsPerPixel;
	file.WriteExact(header, sizeof(header));

	if (isMip) {
		const LegoU16 flags = *(LegoU16*) ((LegoU8*) p_image + 0x0e);
		metadata[0] = 0x31464941;
		metadata[1] = p_image->m_mipLevels;
		metadata[2] = (flags >> 9) & 1;
		metadata[3] = (flags >> 10) & 1;
		metadata[4] = (flags >> 11) & 1;
		file.WriteExact(metadata, 20);
	}

	if (p_image->m_bitsPerPixel == 8) {
		const LegoU32 paletteEntries = *(LegoU16*) ((LegoU8*) p_image + 0x0e) & 0x1ff;
		const LegoU32 colorMapEntryBytes = header[7] >> 3;
		const LegoU8* palette = (const LegoU8*) p_image->m_palette;
		for (LegoU32 i = 0; i < paletteEntries; i++) {
			file.WriteExact(palette + i * 4, colorMapEntryBytes);
		}
	}

	file.WriteExact(p_image->m_pixels, GetMipByteCount(p_image, p_image->m_bitsPerPixel >> 3));
	return TRUE;
}
