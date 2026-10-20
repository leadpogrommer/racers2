#include "render/core/renderer.h"

// STUB: LEGORACERS2 0x004dc7f0
void DisplayRenderer::SetDepthState(LegoU32 p_enabled)
{
	if (p_enabled == 1) {
		SetCachedRenderState(D3DRS_ZENABLE, 1);
		SetCachedRenderState(D3DRS_ZWRITEENABLE, 1);
		SetCachedRenderState(D3DRS_ZFUNC, 4);
	}
	else {
		SetCachedRenderState(D3DRS_ZENABLE, 0);
		SetCachedRenderState(D3DRS_ZWRITEENABLE, 0);
	}
}

// STUB: LEGORACERS2 0x004dccc0
void DisplayRenderer::SetAlphaBlendMode(LegoU32 p_mode)
{
	if (m_useBlendModeOverride == 1) {
		p_mode = m_blendModeOverride;
	}

	if (p_mode == 0) {
		SetCachedRenderState(D3DRS_ALPHABLENDENABLE, 0);
		SetCachedRenderState(D3DRS_ALPHATESTENABLE, 1);
		SetCachedRenderState(D3DRS_ZWRITEENABLE, 1);
		return;
	}

	LegoU32 sourceBlend;
	LegoU32 destinationBlend;
	LegoBool32 hasBlendPair = TRUE;

	switch (p_mode) {
	case 1:
	case 7:
		sourceBlend = D3DBLEND_SRCALPHA;
		destinationBlend = D3DBLEND_INVSRCALPHA;
		break;
	case 2:
		sourceBlend = D3DBLEND_ONE;
		destinationBlend = D3DBLEND_INVSRCCOLOR;
		break;
	case 3:
		sourceBlend = D3DBLEND_SRCALPHA;
		destinationBlend = D3DBLEND_ONE;
		break;
	case 4:
		sourceBlend = D3DBLEND_ONE;
		destinationBlend = D3DBLEND_ONE;
		break;
	case 5:
		sourceBlend = D3DBLEND_ZERO;
		destinationBlend = D3DBLEND_INVSRCALPHA;
		break;
	case 6:
		sourceBlend = D3DBLEND_ZERO;
		destinationBlend = D3DBLEND_INVSRCCOLOR;
		break;
	default:
		hasBlendPair = FALSE;
		break;
	}

	if (hasBlendPair) {
		SetCachedRenderState(D3DRS_SRCBLEND, sourceBlend);
		SetCachedRenderState(D3DRS_DESTBLEND, destinationBlend);
	}

	SetCachedRenderState(D3DRS_ALPHABLENDENABLE, 1);
	SetCachedRenderState(D3DRS_ALPHATESTENABLE, 0);
	SetCachedRenderState(D3DRS_ZWRITEENABLE, 0);
}
