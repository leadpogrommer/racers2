#include "video/moviefilter.h"

#include "decomp.h"
#include "render/core/renderer.h"

#include <math.h>
#include <string.h>

static const GUID kMovieRendererClass = {0x71771540, 0x2017, 0x11cf, {0xae, 0x26, 0x00, 0x20, 0xaf, 0xd7, 0x97, 0x67}};

static const GUID kVideoInfoFormat = {0x05589f80, 0xc356, 0x11ce, {0xbf, 0x01, 0x00, 0xaa, 0x00, 0x55, 0x59, 0x5a}};
static const GUID kVideoMediaType = {0x73646976, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};
static const GUID kRgb24Subtype = {0xe436eb7d, 0x524f, 0x11ce, {0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}};

static LegoU32& MovieDword(MovieTextureRenderer* p_this, LegoU32 p_offset)
{
	return *(LegoU32*) (((LegoU8*) p_this) + p_offset);
}

static LegoU32 MovieDword(const MovieTextureRenderer* p_this, LegoU32 p_offset)
{
	return *(const LegoU32*) (((const LegoU8*) p_this) + p_offset);
}

static IDirect3DTexture8** MovieTextureArray(MovieTextureRenderer* p_this, LegoU32 p_offset)
{
	return (IDirect3DTexture8**) (((LegoU8*) p_this) + p_offset);
}

// STUB: LEGORACERS2 0x00499920
MovieTextureRenderer::MovieTextureRenderer(LPUNKNOWN p_outer, HRESULT* p_result)
	: CBaseVideoRenderer(kMovieRendererClass, NULL, p_outer, p_result)
{
	MovieDword(this, 0x194) = 0;
	for (LegoU32 textureSlot = 0; textureSlot < 8; ++textureSlot) {
		MovieTextureArray(this, 0x154)[textureSlot] = NULL;
		MovieTextureArray(this, 0x174)[textureSlot] = NULL;
	}
	for (LegoU32 vertexSlot = 0; vertexSlot < 4; ++vertexSlot) {
		memset(((LegoU8*) this) + 0x1b8 + vertexSlot * 0x20, 0, 0x10);
	}
	if (p_result != NULL) {
		*p_result = S_OK;
	}
}

// STUB: LEGORACERS2 0x00499a00
MovieTextureRenderer::~MovieTextureRenderer()
{
	LegoS32 count = (LegoS32) MovieDword(this, 0x194);
	for (LegoS32 i = count - 1; i >= 0; --i) {
		IUnknown* texture = (IUnknown*) MovieTextureArray(this, 0x174)[i];
		if (texture != NULL) {
			texture->Release();
		}
		texture = (IUnknown*) MovieTextureArray(this, 0x154)[i];
		if (texture != NULL) {
			texture->Release();
		}
	}
}

// STUB: LEGORACERS2 0x00499a70
HRESULT MovieTextureRenderer::CheckMediaType(const CMediaType* p_mediaType)
{
	if (memcmp(p_mediaType->FormatType(), &kVideoInfoFormat, sizeof(GUID)) != 0) {
		return S_FALSE;
	}
	if (memcmp(&p_mediaType->majortype, &kVideoMediaType, sizeof(GUID)) != 0) {
		return S_FALSE;
	}
	if (memcmp(&p_mediaType->subtype, &kRgb24Subtype, sizeof(GUID)) != 0) {
		return S_FALSE;
	}
	return S_OK;
}

// STUB: LEGORACERS2 0x00499ac0
HRESULT MovieTextureRenderer::SetMediaType(const CMediaType* p_mediaType)
{
	MovieDword(this, 0x250) = 0;
	const VIDEOINFOHEADER* videoInfo = (const VIDEOINFOHEADER*) p_mediaType->Format();
	const BITMAPINFOHEADER* bitmap = &videoInfo->bmiHeader;
	LegoU32 bitCount = bitmap->biBitCount;
	if (bitCount == 24) {
		MovieDword(this, 0x1a0) = 3;
	}
	else if (bitCount == 32) {
		MovieDword(this, 0x1a0) = 4;
	}
	else {
		return S_OK;
	}

	DisplayRenderer* renderer = GetStaticDisplayRenderer();
	static const D3DFORMAT formats[7] = {
		(D3DFORMAT) 0x16,
		(D3DFORMAT) 0x15,
		(D3DFORMAT) 0x17,
		(D3DFORMAT) 0x18,
		(D3DFORMAT) 0x19,
		(D3DFORMAT) 0x1a,
		(D3DFORMAT) 0x1e
	};
	LegoU32 formatIndex;
	for (formatIndex = 0; formatIndex < 7; ++formatIndex) {
		if (renderer->IsTextureFormatSupported(formats[formatIndex])) {
			MovieDword(this, 0x1a4) = formats[formatIndex];
			break;
		}
	}
	if (formatIndex == 7) {
		return S_OK;
	}

	((PixelFormatDescription*) (((LegoU8*) this) + 0x230))->Initialize((D3DFORMAT) MovieDword(this, 0x1a4));
	MovieDword(this, 0x148) = bitmap->biWidth;
	LegoS32 imageHeight = bitmap->biHeight;
	if (imageHeight < 0) {
		imageHeight = -imageHeight;
	}
	MovieDword(this, 0x14c) = imageHeight;
	MovieDword(this, 0x150) = (MovieDword(this, 0x1a0) * MovieDword(this, 0x148) + 3) & 0xfffffffc;
	MovieDword(this, 0x1a8) = MovieDword(this, 0x148);
	MovieDword(this, 0x1ac) = MovieDword(this, 0x14c);
	renderer->AdjustTextureDimensions((LegoU32*) (((LegoU8*) this) + 0x1a8), (LegoU32*) (((LegoU8*) this) + 0x1ac), 0);

	MovieDword(this, 0x198) = 0;
	MovieDword(this, 0x19c) = 0;
	if ((LegoS32) MovieDword(this, 0x148) > 0) {
		LegoS32 extent = 0;
		do {
			++MovieDword(this, 0x198);
			extent += (LegoS32) MovieDword(this, 0x1a8);
		} while (extent < (LegoS32) MovieDword(this, 0x148));
	}
	if ((LegoS32) MovieDword(this, 0x14c) > 0) {
		LegoS32 extent = 0;
		do {
			++MovieDword(this, 0x19c);
			extent += (LegoS32) MovieDword(this, 0x1ac);
		} while (extent < (LegoS32) MovieDword(this, 0x14c));
	}
	LegoS32 textureCount = (LegoS32) MovieDword(this, 0x198) * (LegoS32) MovieDword(this, 0x19c);
	MovieDword(this, 0x194) = textureCount;

	IDirect3DDevice8* device = renderer->m_device;
	for (LegoS32 i = 0; i < textureCount; ++i) {
		device->CreateTexture(
			MovieDword(this, 0x1a8),
			MovieDword(this, 0x1ac),
			1,
			0,
			(D3DFORMAT) MovieDword(this, 0x1a4),
			D3DPOOL_SYSTEMMEM,
			&MovieTextureArray(this, 0x154)[i]
		);
		device->CreateTexture(
			MovieDword(this, 0x1a8),
			MovieDword(this, 0x1ac),
			1,
			0,
			(D3DFORMAT) MovieDword(this, 0x1a4),
			D3DPOOL_DEFAULT,
			&MovieTextureArray(this, 0x174)[i]
		);
	}

	IDirect3DSurface8* backBuffer = NULL;
	D3DSURFACE_DESC backBufferDescription;
	device->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);
	backBuffer->GetDesc(&backBufferDescription);
	MovieDword(this, 0x26c) = backBufferDescription.Width;
	MovieDword(this, 0x270) = backBufferDescription.Height;
	backBuffer->Release();

	LegoU8* vertices = ((LegoU8*) this) + 0x1bc;
	for (LegoU32 vertexSlot = 0; vertexSlot < 4; ++vertexSlot) {
		*(LegoU32*) (vertices - 4) = 0x3f000000;
		*(LegoU32*) vertices = 0x40000000;
		*(LegoU32*) (vertices + 4) = 0xffffffff;
		*(LegoU32*) (vertices + 8) = 0;
		vertices += 0x20;
	}
	MovieDword(this, 0x250) = 1;
	return S_OK;
}

// STUB: LEGORACERS2 0x00499d30
HRESULT MovieTextureRenderer::DoRenderSample(IMediaSample* p_sample)
{
	IDirect3DTexture8** systemTextures = MovieTextureArray(this, 0x154);
	if (systemTextures[0] == NULL) {
		return S_OK;
	}

	PixelFormatDescription format = *(PixelFormatDescription*) (((LegoU8*) this) + 0x230);
	LegoU32 sourceBytesPerPixel = MovieDword(this, 0x1a0);
	BYTE* sampleData = NULL;
	p_sample->GetPointer(&sampleData);

	LegoS32 sourceWidth = (LegoS32) MovieDword(this, 0x148);
	LegoS32 sourceHeight = (LegoS32) MovieDword(this, 0x14c);
	LegoS32 textureWidth = (LegoS32) MovieDword(this, 0x1a8);
	LegoS32 textureHeight = (LegoS32) MovieDword(this, 0x1ac);
	LegoS32 sourcePitch = (LegoS32) MovieDword(this, 0x150);
	LegoS32 tileIndex = 0;
	for (LegoS32 tileY = 0; tileY < (LegoS32) MovieDword(this, 0x19c); ++tileY) {
		for (LegoS32 tileX = 0; tileX < (LegoS32) MovieDword(this, 0x198); ++tileX, ++tileIndex) {
			D3DLOCKED_RECT locked;
			HRESULT result = systemTextures[tileIndex]->LockRect(0, &locked, NULL, 0);
			if (result != D3D_OK) {
				return E_FAIL;
			}

			LegoS32 copyWidth = sourceWidth - textureWidth * tileX;
			if (copyWidth > textureWidth) {
				copyWidth = textureWidth;
			}
			LegoS32 copyHeight = sourceHeight - textureHeight * tileY;
			if (copyHeight > textureHeight) {
				copyHeight = textureHeight;
			}
			const LegoU8* source =
				sampleData + ((sourceWidth * textureHeight * tileY) + textureWidth * tileX) * sourceBytesPerPixel;
			LegoU8* destination = (LegoU8*) locked.pBits;
			LegoS32 sourceRowSkip = sourcePitch - sourceBytesPerPixel * copyWidth;
			LegoS32 destinationRowSkip = locked.Pitch - format.m_bytesPerPixel * copyWidth;

			if (sourceBytesPerPixel >= 3 && format.m_bytesPerPixel == 4) {
				LegoS32 pixelCount = copyWidth - 1;
				for (LegoS32 y = 0; y < copyHeight; ++y) {
					LegoU32 previousPixel = 0;
					for (LegoS32 x = 0; x < pixelCount; ++x) {
						previousPixel = *(const LegoU32*) source;
						*(LegoU32*) destination = previousPixel;
						destination += 4;
						source += sourceBytesPerPixel;
					}
					// Retail retains the preceding DWORD's second byte as tail alpha.
					LegoU32 tail = source[0] | ((LegoU32) source[1] << 8) | ((LegoU32) source[2] << 16) |
								   ((previousPixel & 0x0000ff00) << 16);
					*(LegoU32*) destination = tail;
					source += sourceRowSkip + sourceBytesPerPixel;
					destination += destinationRowSkip + 4;
				}
			}
			else if (sourceBytesPerPixel >= 3 && format.m_bytesPerPixel == 2) {
				for (LegoS32 y = 0; y < copyHeight; ++y) {
					for (LegoS32 x = 0; x < copyWidth; ++x) {
						LegoU32 pixel = ((source[0] >> format.m_blueLoss) << format.m_blueShift) |
										((source[1] >> format.m_greenLoss) << format.m_greenShift) |
										((source[2] >> format.m_redLoss) << format.m_redShift);
						*(LegoU16*) destination = (LegoU16) pixel;
						source += sourceBytesPerPixel;
						destination += 2;
					}
					source += sourceRowSkip;
					destination += destinationRowSkip;
				}
			}

			result = systemTextures[tileIndex]->UnlockRect(0);
			if (result != D3D_OK) {
				return E_FAIL;
			}
		}
	}

	DisplayRenderer* renderer = GetStaticDisplayRenderer();
	IDirect3DDevice8* device = renderer->m_device;
	if (device->BeginScene() == D3D_OK) {
		renderer->SetDepthState(0);
		renderer->SetAlphaBlendMode(0);
		if (renderer->m_cachedRenderStates[15] != 0) {
			renderer->m_cachedRenderStates[15] = 0;
			device->SetRenderState((D3DRENDERSTATETYPE) 15, 0);
		}
		renderer->m_cachedStreamBuffer = NULL;
		if (renderer->m_cachedVertexShader != 0x1c4) {
			renderer->m_cachedVertexShader = 0x1c4;
			device->SetVertexShader(0x1c4);
		}

		float pixelsPerX = (float) MovieDword(this, 0x26c) / (float) sourceWidth;
		float pixelsPerY = (float) MovieDword(this, 0x270) / (float) sourceHeight;
		float yStart = 0.0f;
		for (LegoS32 tileY = 0; tileY < (LegoS32) MovieDword(this, 0x19c); ++tileY) {
			LegoS32 remainingHeight = sourceHeight - ((LegoS32) MovieDword(this, 0x19c) - tileY - 1) * textureHeight;
			float vExtent = remainingHeight < textureHeight ? (float) remainingHeight / (float) textureHeight : 1.0f;
			float yExtent = textureHeight * vExtent * pixelsPerY;
			float xStart = 0.0f;
			for (LegoS32 tileX = 0; tileX < (LegoS32) MovieDword(this, 0x198); ++tileX) {
				LegoS32 remainingWidth = sourceWidth - textureWidth * tileX;
				float uExtent = remainingWidth < textureWidth ? (float) remainingWidth / (float) textureWidth : 1.0f;
				float xExtent = textureWidth * uExtent * pixelsPerX;
				LegoU32 index = tileX + tileY * MovieDword(this, 0x198);
				LegoU8* vertices = ((LegoU8*) this) + 0x1b0;
				*(float*) (vertices + 0x00) = xStart;
				*(float*) (vertices + 0x04) = yStart;
				*(float*) (vertices + 0x20) = xExtent + xStart;
				*(float*) (vertices + 0x24) = yStart;
				*(float*) (vertices + 0x40) = xStart;
				*(float*) (vertices + 0x44) = yStart + yExtent;
				*(float*) (vertices + 0x60) = xExtent + xStart;
				*(float*) (vertices + 0x64) = yStart + yExtent;
				*(float*) (vertices + 0x18) = 0.0f;
				*(float*) (vertices + 0x38) = uExtent;
				*(float*) (vertices + 0x58) = 0.0f;
				*(float*) (vertices + 0x78) = uExtent;
				*(float*) (vertices + 0x1c) = vExtent;
				*(float*) (vertices + 0x3c) = vExtent;
				*(float*) (vertices + 0x5c) = 0.0f;
				*(float*) (vertices + 0x7c) = 0.0f;
				device->SetTexture(0, MovieTextureArray(this, 0x174)[index]);
				device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, 0x20);
				xStart += xExtent;
			}
			yStart += yExtent;
		}
		device->SetTexture(0, NULL);
	}
	device->EndScene();
	renderer->Present();

	IDirect3DTexture8** defaultTextures = MovieTextureArray(this, 0x174);
	for (LegoS32 i = 0; i < (LegoS32) MovieDword(this, 0x194); ++i) {
		device->UpdateTexture(systemTextures[i], defaultTextures[i]);
	}
	return S_OK;
}

// STUB: LEGORACERS2 0x0049a210
void __fastcall ClearMovieTextures(MovieTextureRenderer* p_renderer, void* p_edx)
{
	(void) p_edx;
	IDirect3DTexture8** textures = MovieTextureArray(p_renderer, 0x154);
	if (textures[0] == NULL) {
		return;
	}
	LegoS32 bytesPerRow = (LegoS32) MovieDword(p_renderer, 0x24c) * (LegoS32) MovieDword(p_renderer, 0x1a8);
	for (LegoS32 i = 0; i < (LegoS32) MovieDword(p_renderer, 0x194); ++i) {
		D3DLOCKED_RECT locked;
		if (textures[i]->LockRect(0, &locked, NULL, 0) != D3D_OK) {
			return;
		}
		LegoU8* destination = (LegoU8*) locked.pBits;
		for (LegoS32 row = 0; row < (LegoS32) MovieDword(p_renderer, 0x1ac); ++row) {
			memset(destination, 0, bytesPerRow);
			destination += locked.Pitch;
		}
		if (textures[i]->UnlockRect(0) != D3D_OK) {
			return;
		}
		GetStaticDisplayRenderer()->m_device->UpdateTexture(textures[i], MovieTextureArray(p_renderer, 0x174)[i]);
	}
}
