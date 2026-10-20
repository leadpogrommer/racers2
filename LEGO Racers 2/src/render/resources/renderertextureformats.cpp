#include "render/core/renderer.h"

// FUNCTION: LEGORACERS2 0x004ac110
LegoS32 DisplayRenderer::GetTextureQuality()
{
	return m_textureQuality;
}

// STUB: LEGORACERS2 0x004d8800
void DisplayRenderer::AddSupportedTextureFormat(D3DFORMAT p_format)
{
	if (m_textureFormatCount < 64) {
		RendererTextureFormat* format = &m_textureFormats[m_textureFormatCount];
		format->m_selectedAsCompressed = 0;
		format->m_format = p_format;
		format->m_description.Initialize(p_format);
		m_textureFormatCount++;
	}
}

// FUNCTION: LEGORACERS2 0x004d8840
LegoBool32 DisplayRenderer::InitializeTextureFormats()
{
	D3DFORMAT formats[13] = {
		D3DFMT_R8G8B8,
		D3DFMT_A8R8G8B8,
		D3DFMT_X8R8G8B8,
		D3DFMT_R5G6B5,
		D3DFMT_X1R5G5B5,
		D3DFMT_A1R5G5B5,
		D3DFMT_A4R4G4B4,
		D3DFMT_X4R4G4B4,
		D3DFMT_DXT1,
		D3DFMT_DXT2,
		D3DFMT_DXT3,
		D3DFMT_DXT4,
		D3DFMT_DXT5
	};
	for (LegoS32 i = 0; i < 13; i++) {
		if (m_direct3D->CheckDeviceFormat(
				m_selectedAdapter->m_adapterIndex,
				D3DDEVTYPE_HAL,
				m_modeFormat,
				0,
				D3DRTYPE_TEXTURE,
				formats[i]
			) == D3D_OK) {
			AddSupportedTextureFormat(formats[i]);
		}
	}
	for (i = 0; i < 8; i++) {
		RendererTextureFormat* compressed = &m_textureFormats[SelectTextureFormat(i, 1, 0)];
		m_selectedCompressedTextureFormats[i] = compressed;
		m_selectedTextureFormats[i] = &m_textureFormats[SelectTextureFormat(i, 0, compressed->m_selectedAsCompressed)];
	}
	return TRUE;
}

// STUB: LEGORACERS2 0x004da240
LegoS32 DisplayRenderer::SelectTextureFormat(LegoU32 p_flags, LegoU32 p_compressed, LegoU32 p_force32Bit)
{
	LegoS32 selected = 0;
	LegoU32 bestGreen = 0;
	LegoU32 bestAlpha = 0;
	LegoS32 desiredBits = 32;
	switch (p_flags & 3) {
	case 0:
	case 1:
		desiredBits = 16;
		break;
	case 2:
		desiredBits = 24;
		break;
	case 3:
		desiredBits = 32;
		break;
	}

	if (p_compressed == 0 && p_force32Bit == 1) {
		desiredBits = 32;
	}
	else {
		if (p_compressed == 1) {
			D3DFORMAT opaquePreference[5] = {D3DFMT_DXT3, D3DFMT_DXT1, D3DFMT_DXT5, D3DFMT_DXT2, D3DFMT_DXT4};
			// The repeated DXT4 is present in the retail preference list.
			D3DFORMAT alphaPreference[5] = {D3DFMT_DXT3, D3DFMT_DXT5, D3DFMT_DXT2, D3DFMT_DXT4, D3DFMT_DXT4};
			D3DFORMAT* preference = (p_flags & 4) != 0 ? alphaPreference : opaquePreference;
			for (LegoS32 priority = 0; priority < 5; priority++) {
				for (LegoS32 i = 0; i < m_textureFormatCount; i++) {
					if (m_textureFormats[i].m_format == preference[priority]) {
						m_textureFormats[i].m_selectedAsCompressed = 1;
						return i;
					}
				}
			}
		}
		if (p_force32Bit == 0 && (m_vtable->m_textureQuality(this, NULL) >= 5 || m_unk0x4d8 >= 6) && desiredBits > 16) {
			desiredBits = 16;
		}
	}

	if ((p_flags & 4) != 0) {
		for (LegoS32 bits = desiredBits; bits <= 32 && bestAlpha == 0; bits += 8) {
			for (LegoS32 i = 0; i < m_textureFormatCount; i++) {
				const PixelFormatDescription& description = m_textureFormats[i].m_description;
				LegoU32 alpha = description.m_alphaMask >> (description.m_alphaShift & 31);
				LegoU32 green = description.m_greenMask >> (description.m_greenShift & 31);
				if (alpha != 0 && alpha > bestAlpha && description.m_bitsPerPixel <= bits && alpha <= green) {
					bestAlpha = alpha;
					selected = i;
				}
			}
		}
		if (bestAlpha != 0) {
			return selected;
		}
	}

	for (; desiredBits <= 32 && bestGreen == 0; desiredBits += 8) {
		for (LegoS32 i = 0; i < m_textureFormatCount; i++) {
			const PixelFormatDescription& description = m_textureFormats[i].m_description;
			LegoU32 green = description.m_greenMask >> (description.m_greenShift & 31);
			if (description.m_bitsPerPixel <= desiredBits && green > bestGreen && description.m_alphaMask != 0 &&
				(GetChannelMaskBitCount(green) >= 8 ||
				 description.m_alphaMask >> (description.m_alphaShift & 31) < 32)) {
				bestGreen = green;
				selected = i;
			}
		}
	}
	return selected;
}
