#include "pixelformat.h"

static LegoU32 GetChannelMaskLoss(LegoU32 p_normalizedMask);

// STUB: LEGORACERS2 0x004ae660
void PixelFormatDescription::Initialize(D3DFORMAT p_format)
{
	switch (p_format) {
	case D3DFMT_R8G8B8:
		m_redMask = 0x00ff0000;
		m_greenMask = 0x0000ff00;
		m_blueMask = 0x000000ff;
		m_alphaMask = 0;
		m_bitsPerPixel = 24;
		break;
	case D3DFMT_A8R8G8B8:
		m_redMask = 0x00ff0000;
		m_greenMask = 0x0000ff00;
		m_blueMask = 0x000000ff;
		m_alphaMask = 0xff000000;
		m_bitsPerPixel = 32;
		break;
	case D3DFMT_X8R8G8B8:
		m_redMask = 0x00ff0000;
		m_greenMask = 0x0000ff00;
		m_blueMask = 0x000000ff;
		m_alphaMask = 0;
		m_bitsPerPixel = 32;
		break;
	case D3DFMT_R5G6B5:
		m_redMask = 0x0000f800;
		m_greenMask = 0x000007e0;
		m_blueMask = 0x0000001f;
		m_alphaMask = 0;
		m_bitsPerPixel = 16;
		break;
	case D3DFMT_X1R5G5B5:
		m_redMask = 0x00007c00;
		m_greenMask = 0x000003e0;
		m_blueMask = 0x0000001f;
		m_alphaMask = 0;
		m_bitsPerPixel = 16;
		break;
	case D3DFMT_A1R5G5B5:
		m_redMask = 0x00007c00;
		m_greenMask = 0x000003e0;
		m_blueMask = 0x0000001f;
		m_alphaMask = 0x00008000;
		m_bitsPerPixel = 16;
		break;
	case D3DFMT_A4R4G4B4:
		m_redMask = 0x00000f00;
		m_greenMask = 0x000000f0;
		m_blueMask = 0x0000000f;
		m_alphaMask = 0x0000f000;
		m_bitsPerPixel = 16;
		break;
	case D3DFMT_R3G3B2:
		m_redMask = 0x000000e0;
		m_greenMask = 0x0000001c;
		m_blueMask = 0x00000003;
		m_alphaMask = 0;
		m_bitsPerPixel = 8;
		break;
	case D3DFMT_A8:
		m_redMask = 0;
		m_greenMask = 0;
		m_blueMask = 0;
		m_alphaMask = 0x000000ff;
		m_bitsPerPixel = 8;
		break;
	case D3DFMT_A8R3G3B2:
		m_redMask = 0x000000e0;
		m_greenMask = 0x0000001c;
		m_blueMask = 0x00000003;
		m_alphaMask = 0x0000ff00;
		m_bitsPerPixel = 16;
		break;
	case D3DFMT_X4R4G4B4:
		m_redMask = 0x00000f00;
		m_greenMask = 0x000000f0;
		m_blueMask = 0x0000000f;
		m_alphaMask = 0;
		m_bitsPerPixel = 16;
		break;
	case 0:
	case D3DFMT_A8P8:
	case D3DFMT_P8:
	case 0x32:
	case 0x33:
	case 0x34:
	case D3DFMT_DXT1:
	case D3DFMT_DXT2:
	case D3DFMT_DXT3:
	case D3DFMT_DXT4:
	case D3DFMT_DXT5:
	case D3DFMT_YUY2:
	case D3DFMT_UYVY:
	case 0x3c:
	case 0x3d:
	case 0x3e:
	case 0x3f:
	case 0x40:
	case 0x41:
	case 0x46:
	case 0x47:
	case 0x49:
	case 0x4b:
	case 0x4d:
	case 0x4f:
	case 0x50:
	case 0x64:
	case 0x65:
	case 0x66:
		m_redMask = 0;
		m_greenMask = 0;
		m_blueMask = 0;
		m_alphaMask = 0;
		m_bitsPerPixel = 0;
		break;
	}

	LegoU32 redMask = m_redMask;
	LegoU32 greenMask = m_greenMask;
	LegoU32 blueMask = m_blueMask;

	m_redShift = 0;
	m_greenShift = 0;
	m_blueShift = 0;
	m_alphaShift = 0;

	if (redMask != 0 && greenMask != 0 && blueMask != 0) {
		while ((redMask & 1) == 0) {
			++m_redShift;
			redMask = (LegoU32) ((LegoS32) redMask >> 1);
		}
		while ((greenMask & 1) == 0) {
			++m_greenShift;
			greenMask = (LegoU32) ((LegoS32) greenMask >> 1);
		}
		while ((blueMask & 1) == 0) {
			++m_blueShift;
			blueMask = (LegoU32) ((LegoS32) blueMask >> 1);
		}
	}

	LegoU32 alphaMask = m_alphaMask;
	if (alphaMask != 0) {
		while ((alphaMask & 1) == 0) {
			++m_alphaShift;
			alphaMask = (LegoU32) ((LegoS32) alphaMask >> 1);
		}
	}

	m_redLoss = (LegoU8) GetChannelMaskLoss(m_redMask >> (m_redShift & 0x1f));
	m_greenLoss = (LegoU8) GetChannelMaskLoss(m_greenMask >> (m_greenShift & 0x1f));
	m_blueLoss = (LegoU8) GetChannelMaskLoss(m_blueMask >> (m_blueShift & 0x1f));
	m_alphaLoss = (LegoU8) GetChannelMaskLoss(m_alphaMask >> (m_alphaShift & 0x1f));
	m_bytesPerPixel = m_bitsPerPixel >> 3;
}

// STUB: LEGORACERS2 0x004ae9d0
static LegoU32 GetChannelMaskLoss(LegoU32 p_normalizedMask)
{
	switch (p_normalizedMask) {
	case 0x01:
		return 7;
	case 0x03:
		return 6;
	case 0x07:
		return 5;
	case 0x0f:
		return 4;
	case 0x1f:
		return 3;
	case 0x3f:
		return 2;
	case 0x7f:
		return 1;
	default:
		return 0;
	}
}
