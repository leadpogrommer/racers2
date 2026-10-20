#include "d3dx8.h"
#include "math/scalarutil.h"
#include "render/bitmap/bitmapimage.h"
#include "render/core/renderer.h"
#include "render/resources/rendererrecords.h"

namespace
{
LegoU32 ConvertDirectPixel(const LegoU8* p_pixel, const PixelFormatDescription& p_format, LegoBool32 p_swapRedBlue)
{
	const LegoU8 red = p_pixel[p_swapRedBlue ? 2 : 0];
	const LegoU8 green = p_pixel[1];
	const LegoU8 blue = p_pixel[p_swapRedBlue ? 0 : 2];
	return ((LegoU32) (red >> (p_format.m_redLoss & 0x1f)) << (p_format.m_redShift & 0x1f)) |
		   ((LegoU32) (green >> (p_format.m_greenLoss & 0x1f)) << (p_format.m_greenShift & 0x1f)) |
		   ((LegoU32) (blue >> (p_format.m_blueLoss & 0x1f)) << (p_format.m_blueShift & 0x1f)) |
		   ((LegoU32) (p_pixel[3] >> (p_format.m_alphaLoss & 0x1f)) << (p_format.m_alphaShift & 0x1f));
}
} // namespace

// STUB: LEGORACERS2 0x004d8c90
void DisplayRenderer::CopyBitmapMipToLockedSurface(
	LegoU32 p_mipLevel,
	const BitmapImage* p_image,
	const RendererTextureFormat* p_format,
	const D3DLOCKED_RECT* p_locked
)
{
	const LegoU32 shift = p_mipLevel & 0x1f;
	const LegoS32 width = (LegoS32) p_image->m_width >> shift;
	const LegoS32 height = (LegoS32) p_image->m_height >> shift;
	const LegoS32 outputBytesPerPixel = p_format->m_description.m_bytesPerPixel;
	if (outputBytesPerPixel != 2 && outputBytesPerPixel != 4) {
		return;
	}
	if ((LegoU32) p_image->m_unk0x10 > 2) {
		return;
	}

	const LegoU8* source = p_image->m_pixels;
	if (p_image->m_mipLevels >= 2) {
		LegoS32 sourceOffset = 0;
		const LegoS32 sourceBytesPerPixel = p_image->m_bitsPerPixel >> 3;
		for (LegoU32 level = 0; level < p_mipLevel; level++) {
			const LegoU32 sourceShift = level & 0x1f;
			sourceOffset += ((LegoS32) p_image->m_width >> sourceShift) * ((LegoS32) p_image->m_height >> sourceShift) *
							sourceBytesPerPixel;
		}
		source += sourceOffset;
	}

	const PixelFormatDescription& description = p_format->m_description;
	LegoU32 palette[256];
	if (p_image->m_unk0x10 == 2) {
		const LegoU8* paletteBytes = (const LegoU8*) p_image->m_palette;
		for (LegoS32 i = 0; i < 256; i++) {
			const LegoU8* entry = paletteBytes + i * 4;
			palette[i] =
				((LegoU32) (entry[2] >> (description.m_redLoss & 0x1f)) << (description.m_redShift & 0x1f)) |
				((LegoU32) (entry[1] >> (description.m_greenLoss & 0x1f)) << (description.m_greenShift & 0x1f)) |
				((LegoU32) (entry[0] >> (description.m_blueLoss & 0x1f)) << (description.m_blueShift & 0x1f)) |
				((LegoU32) (entry[3] >> (description.m_alphaLoss & 0x1f)) << (description.m_alphaShift & 0x1f));
		}
	}

	LegoU8* destination = (LegoU8*) p_locked->pBits;
	const LegoS32 rowPadding = p_locked->Pitch - width * outputBytesPerPixel;
	for (LegoS32 y = 0; y < height; y++) {
		for (LegoS32 x = 0; x < width; x++) {
			LegoU32 value;
			if (p_image->m_unk0x10 == 2) {
				value = palette[*source++];
			}
			else {
				value = ConvertDirectPixel(source, description, p_image->m_unk0x10 == 1);
				source += 4;
			}
			if (outputBytesPerPixel == 2) {
				*(LegoU16*) destination = (LegoU16) value;
				destination += 2;
			}
			else {
				*(LegoU32*) destination = value;
				destination += 4;
			}
		}
		destination += rowPadding;
	}
}

// FUNCTION: LEGORACERS2 0x004d9030
LegoS32 DisplayRenderer::GetTextureMipLevelCount(LegoS32 p_width, LegoS32 p_height) const
{
	LegoS32 dimension = (LegoU32) p_width < (LegoU32) p_height ? p_width : p_height;
	LegoS32 levels = 0;
	while (dimension >= 4) {
		dimension >>= 1;
		levels++;
	}
	return levels;
}

// STUB: LEGORACERS2 0x004d9050
LegoBool32 DisplayRenderer::CreatePrimaryTexture(RendererTextureRecord* p_record, LegoBool32 p_compressed)
{
	LegoU32 flags = p_record->m_flags0x9c;
	if ((flags & 0x0c) != 0) {
		p_compressed = FALSE;
	}
	if ((flags & 0x04) != 0) {
		p_record->m_flags0x9c = flags & ~0x10;
		return TRUE;
	}

	LegoU32 formatIndex = (flags >> 6) & 0xffff;
	RendererTextureFormat* format = m_selectedTextureFormats[formatIndex];
	RendererTextureFormat* selected = format;
	if (p_compressed == TRUE) {
		selected = m_selectedCompressedTextureFormats[(p_record->m_flags0x9c >> 6) & 0xffff];
	}

	flags = (flags & ~0x10) | ((selected != format) ? 0x10 : 0);
	p_record->m_flags0x9c = flags;
	if ((flags & 0x08) != 0) {
		g_lastDirect3DResult = m_device->CreateCubeTexture(
			p_record->m_width,
			p_record->m_flags0x90 >> 9,
			0,
			format->m_format,
			D3DPOOL_SYSTEMMEM,
			(IDirect3DCubeTexture8**) &p_record->m_primaryTexture
		);
	}
	else {
		g_lastDirect3DResult = m_device->CreateTexture(
			p_record->m_width,
			p_record->m_height,
			p_record->m_flags0x90 >> 9,
			0,
			format->m_format,
			D3DPOOL_SYSTEMMEM,
			(IDirect3DTexture8**) &p_record->m_primaryTexture
		);
	}
	return g_lastDirect3DResult == D3D_OK ? TRUE : FALSE;
}

// STUB: LEGORACERS2 0x004d9160
LegoBool32 DisplayRenderer::CreateSecondaryTexture(RendererTextureRecord* p_record)
{
	if (p_record->m_primaryTexture == NULL || p_record->m_texture != NULL) {
		return TRUE;
	}

	const LegoU32 flags = p_record->m_flags0x9c;
	HRESULT result;
	if ((flags & 0x04) != 0) {
		if ((flags & 0x08) != 0) {
			result = m_device->CreateCubeTexture(
				p_record->m_width,
				p_record->m_flags0x90 >> 9,
				1,
				m_modeFormat,
				D3DPOOL_DEFAULT,
				(IDirect3DCubeTexture8**) &p_record->m_texture
			);
		}
		else {
			result = m_device->CreateTexture(
				p_record->m_width,
				p_record->m_height,
				p_record->m_flags0x90 >> 9,
				1,
				m_modeFormat,
				D3DPOOL_DEFAULT,
				(IDirect3DTexture8**) &p_record->m_texture
			);
		}
	}
	else {
		const LegoU32 formatIndex = (flags >> 6) & 0xffff;
		RendererTextureFormat* format = (flags & 0x10) != 0 ? m_selectedCompressedTextureFormats[formatIndex]
															: m_selectedTextureFormats[formatIndex];
		if ((flags & 0x08) != 0) {
			result = m_device->CreateCubeTexture(
				p_record->m_width,
				p_record->m_flags0x90 >> 9,
				0,
				format->m_format,
				D3DPOOL_DEFAULT,
				(IDirect3DCubeTexture8**) &p_record->m_texture
			);
		}
		else {
			result = m_device->CreateTexture(
				p_record->m_width,
				p_record->m_height,
				p_record->m_flags0x90 >> 9,
				0,
				format->m_format,
				D3DPOOL_DEFAULT,
				(IDirect3DTexture8**) &p_record->m_texture
			);
		}
	}
	return result == D3D_OK ? TRUE : FALSE;
}

// STUB: LEGORACERS2 0x004d9970
LegoBool32 DisplayRenderer::SetTextureRecordDimensions(
	RendererTextureRecord* p_record,
	LegoU32 p_width,
	LegoU32 p_height,
	LegoU32 p_mipLevels,
	LegoU32 p_flagA,
	LegoU8 p_sevenBitField,
	LegoU32 p_formatMode
)
{
	if (p_width < 4 || p_height < 4) {
		return FALSE;
	}

	LegoU32 requestedMipLevels = p_mipLevels;
	const LegoU32 caps = m_selectedAdapter->m_caps.TextureCaps;
	if (p_formatMode == 0) {
		if ((caps & 0x4000) == 0) {
			requestedMipLevels = 1;
		}
	}
	else if ((caps & 0x10000) == 0) {
		requestedMipLevels = 1;
	}

	AdjustTextureDimensions(&p_width, &p_height, p_formatMode);
	if (requestedMipLevels == 0) {
		requestedMipLevels = 100;
	}
	const LegoS32 maximumMipLevels = GetTextureMipLevelCount((LegoS32) p_width, (LegoS32) p_height);
	if (requestedMipLevels > (LegoU32) maximumMipLevels) {
		requestedMipLevels = (LegoU32) maximumMipLevels;
	}

	LegoBool32 reduced = FALSE;
	if ((m_vtable->m_textureQuality(this, NULL) >= 7 || *(LegoU32*) ((LegoU8*) this + 0x4d8) > 7) &&
		requestedMipLevels > 4) {
		p_width >>= 1;
		p_height >>= 1;
		requestedMipLevels--;
		reduced = TRUE;
	}

	p_record->m_texture = NULL;
	p_record->m_primaryTexture = NULL;
	p_record->m_resourceIndex = 0;
	p_record->m_width = (LegoS16) p_width;
	p_record->m_height = (LegoS16) p_height;
	p_record->m_flags0x90 =
		(LegoU16) (((p_sevenBitField & 0x7f) | (requestedMipLevels << 7)) << 2) | (p_record->m_flags0x90 & 3);
	p_record->m_flags0x9c =
		(((reduced << 2 | (p_formatMode & 1)) << 1 | (p_flagA & 1)) << 2) | (p_record->m_flags0x9c & 0xffffffd3);
	return TRUE;
}

// STUB: LEGORACERS2 0x004d9ae0
void DisplayRenderer::AdjustTextureDimensions(LegoU32* p_width, LegoU32* p_height, LegoU32 p_ignoredFormat)
{
	LegoU32 maximum = m_selectedAdapter->m_caps.MaxTextureWidth;
	if (maximum >= 0x400) {
		maximum = 0x400;
	}

	LegoS32 width = (LegoS32) *p_width;
	LegoS32 height = (LegoS32) *p_height;
	if (width <= 4) {
		width = 4;
	}
	if (height <= 4) {
		height = 4;
	}
	if (width >= (LegoS32) maximum) {
		width = (LegoS32) maximum;
	}
	if (height >= (LegoS32) maximum) {
		height = (LegoS32) maximum;
	}
	*p_width = (LegoU32) RoundUpToPowerOfTwo(width);
	*p_height = (LegoU32) RoundUpToPowerOfTwo(height);

	if ((m_selectedAdapter->m_caps.TextureCaps & 0x20) != 0) {
		LegoU32 side = *p_width > *p_height ? *p_width : *p_height;
		*p_width = side;
		*p_height = side;
	}
}

// STUB: LEGORACERS2 0x004da020
void RendererTextureRecord::UpdateGpuTexture()
{
	if (m_texture == NULL || (m_flags0x9c & 0x04) != 0) {
		return;
	}

	if ((m_flags0x9c & 0x08) != 0) {
		IDirect3DCubeTexture8* primary = (IDirect3DCubeTexture8*) m_primaryTexture;
		for (LegoU32 face = 0; face < 6; face++) {
			primary->AddDirtyRect((D3DCUBEMAP_FACES) face, NULL);
		}
	}
	else {
		((IDirect3DTexture8*) m_primaryTexture)->AddDirtyRect(NULL);
	}

	DisplayRenderer* renderer = GetStaticDisplayRenderer();
	if ((m_flags0x9c & 0x10) == 0) {
		renderer->m_device->UpdateTexture(m_primaryTexture, m_texture);
		return;
	}

	D3DSURFACE_DESC desc;
	IDirect3DSurface8* surface = NULL;
	((IDirect3DTexture8*) m_texture)->GetSurfaceLevel(0, &surface);
	surface->GetDesc(&desc);
	surface->Release();

	const LegoU32 levels = m_flags0x90 >> 9;
	IDirect3DBaseTexture8* conversionTexture = NULL;
	HRESULT result;
	if ((m_flags0x9c & 0x08) != 0) {
		result = renderer->m_device->CreateCubeTexture(
			m_width,
			levels,
			0,
			desc.Format,
			D3DPOOL_SYSTEMMEM,
			(IDirect3DCubeTexture8**) &conversionTexture
		);
	}
	else {
		result = renderer->m_device->CreateTexture(
			m_width,
			m_height,
			levels,
			0,
			desc.Format,
			D3DPOOL_SYSTEMMEM,
			(IDirect3DTexture8**) &conversionTexture
		);
	}
	if (result != D3D_OK) {
		return;
	}

	if ((m_flags0x90 & 0xfe00) != 0) {
		for (LegoU32 level = 0; level < levels; level++) {
			IDirect3DSurface8* source = NULL;
			IDirect3DSurface8* destination = NULL;
			// Retail uses the 2D surface-level ABI here even when its cube
			// bit is set. Normal creation disables compression for cubes.
			((IDirect3DTexture8*) m_primaryTexture)->GetSurfaceLevel(level, &source);
			((IDirect3DTexture8*) conversionTexture)->GetSurfaceLevel(level, &destination);
			D3DXLoadSurfaceFromSurface(destination, NULL, NULL, source, NULL, NULL, 1, 0);
			destination->Release();
			source->Release();
		}
	}

	renderer->m_device->UpdateTexture(conversionTexture, m_texture);
	conversionTexture->Release();
}

// STUB: LEGORACERS2 0x004da540
void RendererTextureRecord::UploadBitmapImage(BitmapImage* p_image)
{
	const LegoU32 formatIndex = (m_flags0x9c >> 6) & 0xffff;
	DisplayRenderer* renderer = GetStaticDisplayRenderer();
	const RendererTextureFormat* format = renderer->m_selectedTextureFormats[formatIndex];
	IDirect3DSurface8* surface = NULL;
	((IDirect3DTexture8*) m_primaryTexture)->GetSurfaceLevel(0, &surface);

	D3DLOCKED_RECT locked;
	if (surface->LockRect(&locked, NULL, 0) == D3D_OK) {
		renderer->CopyBitmapMipToLockedSurface((m_flags0x9c & 0x20) != 0, p_image, format, &locked);
		surface->UnlockRect();
	}
	surface->Release();
	UpdateGpuTexture();
}
