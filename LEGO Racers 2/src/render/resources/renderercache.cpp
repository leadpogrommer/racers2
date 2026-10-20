#include "render/core/renderer.h"

// FUNCTION: LEGORACERS2 0x004adfe0
void DisplayRenderer::SetCachedRenderState(D3DRENDERSTATETYPE p_state, LegoU32 p_value)
{
	if (m_cachedRenderStates[p_state] != p_value) {
		m_cachedRenderStates[p_state] = p_value;
		m_device->SetRenderState(p_state, p_value);
	}
}

// STUB: LEGORACERS2 0x004ae010
void DisplayRenderer::ResetTextureStages()
{
	if (m_device == NULL) {
		return;
	}

	for (LegoU32 stage = 0; stage < 8; ++stage) {
		m_device->SetTexture(stage, NULL);
		// Retail deliberately invalidates this handle cache after unbinding.
		m_cachedTextureHandles[stage] = 0x12345678;
		SetCachedTextureStageState(stage, D3DTSS_COLOROP, D3DTOP_DISABLE);
		SetCachedTextureStageState(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		SetCachedTextureStageState(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
		SetCachedTextureStageState(stage, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		SetCachedTextureStageState(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		SetCachedTextureStageState(stage, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
		SetCachedTextureStageState(stage, D3DTSS_TEXCOORDINDEX, 0);
		SetCachedTextureStageState(stage, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
		SetCachedTextureStageState(stage, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
		SetCachedTextureStageState(stage, D3DTSS_ADDRESSW, D3DTADDRESS_CLAMP);
		SetCachedTextureStageState(stage, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		SetCachedTextureStageState(stage, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		SetCachedTextureStageState(stage, D3DTSS_MIPFILTER, D3DTEXF_ANISOTROPIC);
		SetCachedTextureStageState(stage, D3DTSS_MIPMAPLODBIAS, 0);
		SetCachedTextureStageState(stage, D3DTSS_MAXMIPLEVEL, 0);
		SetCachedTextureStageState(stage, D3DTSS_BORDERCOLOR, 0);
		SetCachedTextureStageState(stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	}

	SetCachedTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	SetCachedTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
}

// STUB: LEGORACERS2 0x004ae310
void DisplayRenderer::SetCachedTextureStageState(LegoU32 p_stage, D3DTEXTURESTAGESTATETYPE p_state, LegoU32 p_value)
{
	LegoU32 index = (p_stage << 5) | (LegoU32) p_state;
	if (m_cachedTextureStageStates[index] != p_value) {
		m_cachedTextureStageStates[index] = p_value;
		m_device->SetTextureStageState(p_stage, p_state, p_value);
	}
}
