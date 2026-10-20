#include "render/core/renderer.h"

// FUNCTION: LEGORACERS2 0x004dc510
void DisplayRenderer::SetTextureQuality(LegoU32 p_quality)
{
	m_textureQuality = p_quality;
	for (LegoU32 kind = 0; kind < 8; kind++) {
		RendererTextureFormat* compressed = &m_textureFormats[SelectTextureFormat(kind, 1, 0)];
		m_selectedCompressedTextureFormats[kind] = compressed;
		m_selectedTextureFormats[kind] =
			&m_textureFormats[SelectTextureFormat(kind, 0, compressed->m_selectedAsCompressed)];
	}
}

// STUB: LEGORACERS2 0x004dc6e0
void DisplayRenderer::SetFogEnabled(LegoU32 p_enabled)
{
	SetCachedRenderState(D3DRS_FOGENABLE, p_enabled == 1 ? 1 : 0);
}

// STUB: LEGORACERS2 0x004dc740
void DisplayRenderer::SetFogColor(LegoU32 p_color)
{
	const LegoU32 rgb = ((p_color & 0xff) << 16) | (p_color & 0xff00) | ((p_color >> 16) & 0xff);
	SetCachedRenderState(D3DRS_FOGCOLOR, rgb);
}

// STUB: LEGORACERS2 0x004dc790
void DisplayRenderer::SetFogRange(LegoFloat p_start, LegoFloat p_end)
{
	union FloatBits {
		LegoFloat m_float;
		LegoU32 m_bits;
	};
	FloatBits start;
	FloatBits end;
	start.m_float = p_start;
	end.m_float = p_end;
	SetCachedRenderState(D3DRS_FOGSTART, start.m_bits);
	SetCachedRenderState(D3DRS_FOGEND, end.m_bits);
}

// STUB: LEGORACERS2 0x004dc8c0
void DisplayRenderer::SetAlphaTestReference(LegoU32 p_reference)
{
	if (p_reference > 16) {
		p_reference = 16;
	}
	SetCachedRenderState(D3DRS_ALPHAREF, p_reference);
}

// STUB: LEGORACERS2 0x004dc900
void DisplayRenderer::SetDepthAlwaysWrite()
{
	SetCachedRenderState(D3DRS_ZWRITEENABLE, 1);
	SetCachedRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
}

// FUNCTION: LEGORACERS2 0x004dc960
void DisplayRenderer::AdjustSolidPassScope(LegoU32 p_enter)
{
	if (p_enter != 0) {
		++m_unk0xc6268;
	}
	else {
		--m_unk0xc6268;
	}
}

// STUB: LEGORACERS2 0x004dc990
LegoBool32 DisplayRenderer::IsSolidPassOverrideActive()
{
	return m_fillMode == D3DFILL_SOLID && m_unk0xc6268 != 0;
}
