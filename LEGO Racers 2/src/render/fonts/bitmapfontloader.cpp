#include "math/scalarutil.h"
#include "memory/memoryheap.h"
#include "render/bitmap/bitmapimage.h"
#include "render/bitmap/bitmaputil.h"
#include "render/core/renderer.h"
#include "render/core/rendererconstructiondispatch.h"
#include "render/fonts/bitmapfont.h"
#include "render/resources/rendererrecords.h"

#include <string.h>

namespace
{
struct BitmapFontPageUploadDispatch {
	undefined m_unk0x00[0xd4];
	RendererMainDispatch::CreateTextureRecord m_createTextureRecord;
	undefined m_unk0xd8[0x114 - 0xd8];
	RendererMainDispatch::TextureRecordIndex m_getTextureRecord;
};

LegoBool32 UploadFontPage(
	BitmapFontPageUploadDispatch* p_dispatch,
	DisplayRenderer* p_renderer,
	LegoS32 p_id,
	BitmapImage* p_image
)
{
	LegoU32 output = 0;
	if (!p_dispatch->m_createTextureRecord(
			p_renderer,
			NULL,
			(LegoU32) p_image->m_unk0x10,
			p_id,
			(LegoU32) p_image->m_width,
			(LegoU32) p_image->m_height,
			1,
			&output,
			0
		)) {
		return FALSE;
	}
	RendererTextureRecord* record = p_dispatch->m_getTextureRecord(p_renderer, NULL, p_id);
	record->UploadBitmapImage(p_image);
	return TRUE;
}

LegoS32 GetRoundedAtlasRoot(LegoS32 p_width, LegoS32 p_height)
{
	LegoFloat root;
	LegoS32 roundedRoot;
	// Retail stores the FSQRT result to float, reloads it, then uses FISTP.
	// This preserves the active x87 rounding mode and the intermediate store.
	// clang-format off
	__asm {
		fild p_width
		fimul p_height
		fsqrt
		fstp root
		fld root
		fistp roundedRoot
	}
	// clang-format on
	return roundedRoot;
}
} // namespace

// STUB: LEGORACERS2 0x004c3540
LegoBool32 BitmapFont::Load(
	LegoU32 p_fontIndex,
	const LegoChar* p_path,
	const LegoChar* p_characterMap,
	LegoS32 p_glyphWidth,
	LegoU32 p_caseMode
)
{
	LegoU8 characterMap[256];
	LegoS32 code;
	LegoS32 index;
	memset(characterMap, 0xff, sizeof(characterMap));

	if (p_characterMap == NULL) {
		LegoU8 character = 0;
		for (code = 'A'; code <= 'Z'; code++) {
			characterMap[code] = character++;
		}
		for (code = 'a'; code <= 'z'; code++) {
			characterMap[code] = character++;
		}
		for (code = '0'; code <= '9'; code++) {
			characterMap[code] = character++;
		}
		static const LegoU8 punctuation[] = {
			'(',  ')',  '?',  '.',  ',',  '\'', ':',  '!',  '-',  0xa1, 0xbf, 0xe1, 0xe0, 0xe4, 0xe9, 0xe8, 0xeb,
			0xea, 0xed, 0xec, 0xef, 0xf3, 0xf2, 0xf6, 0xf4, 0xf9, '@',  '/',  '\\', '_',  '+',  '#',  '%',  0xb0,
			'*',  0xc8, '~',  0xdf, 0xfa, 0xfc, 0xe2, 0xee, 0xc1, 0xf1, 0xc4, 0xd6, 0xdc, '^',  '<',
		};
		for (index = 0; index < (LegoS32) sizeOfArray(punctuation); index++) {
			characterMap[punctuation[index]] = character++;
		}
	}
	else {
		LegoU8 character = 0;
		for (const LegoU8* source = (const LegoU8*) p_characterMap; *source != '\0'; source++, character++) {
			LegoU8 code = *source;
			characterMap[code] = character;
			if (p_caseMode == 1) {
				LegoS32 pairedCode = code;
				if (code >= 'a' && code <= 'z') {
					pairedCode -= 0x20;
				}
				else if ((code >= 'A' && code <= 'Z') || (code >= 0xc0 && code <= 0xdf)) {
					pairedCode += 0x20;
				}
				else if (code >= 0xe0) {
					pairedCode -= 0x20;
				}
				else if (code == 0x8c) {
					pairedCode += 0x10;
				}
				else if (code == 0x9c) {
					pairedCode -= 0x10;
				}
				else if (code == 0xb0) {
					pairedCode += 0x0a;
				}
				else if (code == 0xba) {
					pairedCode -= 0x0a;
				}
				if (pairedCode != code) {
					characterMap[(LegoU8) pairedCode] = character;
				}
			}
		}
	}

	BitmapImage image;
	LegoS32 temporaryTextureId = g_displayRenderer->CreateTextureResource("", 0, 0, 0, 1);
	m_textureCount = 0;
	m_textureIds[m_textureCount++] = temporaryTextureId;
	image.LoadTga(p_path, 1);

	LegoS32 width = image.m_width;
	LegoS32 height = image.m_height;
	LegoS32 dimension = GetRoundedAtlasRoot(width, height);
	LegoS32 nextPowerOfTwo = RoundUpToPowerOfTwo(dimension);
	if (dimension < (nextPowerOfTwo >> 1) + (nextPowerOfTwo >> 3)) {
		dimension = nextPowerOfTwo >> 1;
	}
	else {
		dimension = nextPowerOfTwo;
	}
	LegoU32 maximumDimension = g_displayRenderer->GetMaximumTextureDimension();
	if ((LegoU32) dimension >= maximumDimension) {
		dimension = (LegoS32) maximumDimension;
	}
	m_unk0x10 = dimension;

	LegoU32 bytesPerPixel = image.m_bitsPerPixel >> 3;
	MemoryHeap* heap = GetMemoryHeap();
	LegoU32 pageBytes = (LegoU32) (dimension * dimension) * bytesPerPixel;
	LegoU8* page = (LegoU8*) heap->Allocate(pageBytes, "Temp Font Bitmap");
	memset(page, 0, pageBytes);

	BitmapImage pageImage;
	pageImage.m_palette = image.m_palette;
	pageImage.m_pixels = page;
	pageImage.m_width = (LegoS16) dimension;
	pageImage.m_height = (LegoS16) dimension;
	pageImage.m_bitsPerPixel = image.m_bitsPerPixel;
	pageImage.m_mipLevels = 1;
	pageImage.m_paletteEntries = image.m_paletteEntries;
	pageImage.m_unk0x10 = image.m_unk0x10;

	LegoS32 glyphCount = width / p_glyphWidth;
	LegoS32 glyphTop = FindFirstNonemptyRow(image.m_bitsPerPixel, image.m_pixels, width, height - 1, 1, width);
	LegoS32 bottom = FindFirstNonemptyRow(
		image.m_bitsPerPixel,
		image.m_pixels + (height - 1) * width * bytesPerPixel,
		-width,
		height - 1,
		1,
		width
	);
	LegoS32 glyphBottom = height - bottom - 1;
	LegoS32 glyphHeight = glyphBottom - glyphTop;
	LegoS32 packedGlyphHeight = glyphHeight + 3;
	LegoS32 x = 0;
	LegoS32 y = 0;
	LegoS32 rowWidth = 0;
	LegoS32 glyphsOnPage = 0;
	LegoS32 pageIndex = temporaryTextureId;
	LegoS32 glyphIndex = 0;
	LegoS32 sourceGlyphOffset = 0;
	LegoU8* destination = page;
	BitmapFontPageUploadDispatch* rendererDispatch = (BitmapFontPageUploadDispatch*) g_displayRenderer->m_vtable;

	while (glyphIndex < glyphCount) {
		LegoU8* glyphStart = image.m_pixels + sourceGlyphOffset;
		LegoS32 left = FindFirstNonemptyRow(image.m_bitsPerPixel, glyphStart, 1, p_glyphWidth - 1, width, height - 1);
		if (left == -1) {
			break;
		}
		LegoS32 right = FindFirstNonemptyRow(
			image.m_bitsPerPixel,
			glyphStart + (p_glyphWidth - 1) * bytesPerPixel,
			-1,
			p_glyphWidth - 1,
			width,
			height - 1
		);
		LegoS32 glyphPixelWidth = (p_glyphWidth - right) - 1 - left;
		LegoS32 cellWidth = glyphPixelWidth + 3;
		if (cellWidth > rowWidth) {
			rowWidth = cellWidth;
		}
		if (dimension < cellWidth + x) {
			y += packedGlyphHeight;
			x = 0;
		}
		if (dimension < packedGlyphHeight + y) {
			pageImage.m_height = (LegoS16) dimension;
			if (UploadFontPage(rendererDispatch, g_displayRenderer, pageIndex, &pageImage) == FALSE) {
				image.ReleaseStorage();
				return FALSE;
			}
			memset(page, 0, pageBytes);
			pageIndex = g_displayRenderer->CreateTextureResource("", 0, 0, 0, 1);
			m_textureIds[m_textureCount++] = pageIndex;
			x = 0;
			y = 0;
			glyphsOnPage = 0;
		}

		LegoS32 copyRows = packedGlyphHeight - 2;
		LegoS32 copyBytes = (cellWidth - 2) * bytesPerPixel;
		const LegoU8* sourceRow = glyphStart + (glyphBottom * width + left) * bytesPerPixel;
		LegoU8* destinationRow = destination + ((y + 1) * dimension + (x + 1)) * bytesPerPixel;
		for (LegoS32 row = 0; row < copyRows; row++) {
			memcpy(
				destinationRow + row * dimension * bytesPerPixel,
				sourceRow - row * width * bytesPerPixel,
				copyBytes
			);
		}

		BitmapFontGlyph* glyph = &m_glyphs[glyphIndex];
		glyph->m_texture = pageIndex;
		glyph->m_u = (LegoFloat) (x + 1);
		glyph->m_v = (LegoFloat) (y + 1);
		glyph->m_width = (LegoFloat) (cellWidth - 2);
		glyph->m_height = (LegoFloat) copyRows;
		glyph->m_uExtent = (LegoFloat) (cellWidth - 2);
		glyph->m_vExtent = (LegoFloat) copyRows;
		x += cellWidth;
		glyphsOnPage++;
		glyphIndex++;
		sourceGlyphOffset += p_glyphWidth * bytesPerPixel;
	}

	if (glyphsOnPage != 0 || y != 0) {
		LegoS32 pageHeight = 1;
		while (pageHeight < packedGlyphHeight + y) {
			pageHeight <<= 1;
		}
		pageImage.m_height = (LegoS16) pageHeight;
		if (UploadFontPage(rendererDispatch, g_displayRenderer, pageIndex, &pageImage) == FALSE) {
			image.ReleaseStorage();
			return FALSE;
		}
	}

	heap->Free(page);
	image.ReleaseStorage();

	for (index = 0; index < glyphIndex; index++) {
		BitmapFontGlyph* glyph = &m_glyphs[index];
		LegoFloat reciprocalDimension = 1.0f / dimension;
		glyph->m_u = glyph->m_u * reciprocalDimension + 0.49000000953674316f * reciprocalDimension;
		glyph->m_uExtent = (glyph->m_uExtent - 1.0f) / dimension;
		if (glyph->m_texture == (LegoU32) pageIndex) {
			LegoFloat reciprocalPageHeight = 1.0f / pageImage.m_height;
			glyph->m_v = glyph->m_v * reciprocalPageHeight + 0.49000000953674316f * reciprocalPageHeight;
			glyph->m_vExtent = (glyph->m_vExtent - 1.0f) / pageImage.m_height;
		}
		else {
			glyph->m_v = glyph->m_v * reciprocalDimension + 0.49000000953674316f * reciprocalDimension;
			glyph->m_vExtent = (glyph->m_vExtent - 1.0f) / dimension;
		}
	}

	m_fontIndex = p_fontIndex;
	m_bitsPerPixel = bytesPerPixel * 8;
	m_glyphCount = glyphIndex;
	m_unk0x0c = 0;
	m_scaleX = 0.0015625000232830644f;
	m_scaleY = 0.0020833334419876337f;
	m_spacing = 1.0f;
	m_unk0x1c = (LegoFloat) (rowWidth - 2);
	m_unk0x20 = (LegoFloat) glyphHeight;
	m_advanceWidth = 0.0f;
	m_unk0x28 = (LegoFloat) (rowWidth - 2) * 0.25f;

	for (index = 0; index < 10; index++) {
		LegoU8 character = (LegoU8) ('0' + index);
		if (characterMap[character] != 0xff && m_advanceWidth < m_glyphs[characterMap[character]].m_width) {
			m_advanceWidth = m_glyphs[characterMap[character]].m_width;
		}
	}
	memcpy(m_characterMap, characterMap, sizeof(characterMap));
	return TRUE;
}
