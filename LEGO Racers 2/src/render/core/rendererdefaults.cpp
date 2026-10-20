#include "render/core/renderer.h"
#include "render/device/displayconfig.h"

// STUB: LEGORACERS2 0x004adba0
void DisplayRenderer::InitializeDefaultRenderStates()
{
	for (LegoU32 stateIndex = 0; stateIndex < sizeOfArray(m_cachedRenderStates); ++stateIndex) {
		m_cachedRenderStates[stateIndex] = 0x12345678;
	}
	for (LegoU32 textureIndex = 0; textureIndex < sizeOfArray(m_cachedTextureStageStates); ++textureIndex) {
		m_cachedTextureStageStates[textureIndex] = 0x12345678;
	}
	for (LegoU32 textureHandleIndex = 0; textureHandleIndex < sizeOfArray(m_cachedTextureHandles);
		 ++textureHandleIndex) {
		m_cachedTextureHandles[textureHandleIndex] = 0x12345678;
	}
	m_cachedStreamBuffer = NULL;
	m_cachedVertexShader = 0;

	if (m_device != NULL) {
		SetCachedRenderState(D3DRS_FILLMODE, m_fillMode);
		SetCachedRenderState(D3DRS_SHADEMODE, 2);
		SetCachedRenderState(D3DRS_ZBIAS, 0);
		SetCachedRenderState(D3DRS_COLORVERTEX, 0);
		SetCachedRenderState(D3DRS_AMBIENTMATERIALSOURCE, 0);
		SetCachedRenderState(D3DRS_DIFFUSEMATERIALSOURCE, 0);
		SetCachedRenderState(D3DRS_SPECULARMATERIALSOURCE, 0);
		SetCachedRenderState(D3DRS_EMISSIVEMATERIALSOURCE, 0);

		if ((LegoS32) m_requestedBitDepth > 16) {
			SetCachedRenderState(D3DRS_DITHERENABLE, 0);
		}
		else {
			SetCachedRenderState(D3DRS_DITHERENABLE, g_displayConfiguration->m_dithering);
		}

		SetCachedRenderState(D3DRS_CULLMODE, m_clockwiseCulling ? D3DCULL_CW : D3DCULL_CCW);
		SetCachedRenderState(D3DRS_EDGEANTIALIAS, 0);
		SetCachedRenderState(D3DRS_SPECULARENABLE, 0);
		SetCachedRenderState(D3DRS_LASTPIXEL, 0);
		SetCachedRenderState(D3DRS_FOGENABLE, 0);
		SetCachedRenderState(D3DRS_FOGCOLOR, 0);
		SetCachedRenderState(D3DRS_FOGTABLEMODE, 3);
		SetCachedRenderState(D3DRS_FOGSTART, 0);
		SetCachedRenderState(D3DRS_FOGEND, 0);

		SetCachedRenderState(D3DRS_RANGEFOGENABLE, 0);
		SetCachedRenderState(D3DRS_FOGVERTEXMODE, 0);
		SetCachedRenderState(D3DRS_ALPHATESTENABLE, 0);
		SetCachedRenderState(D3DRS_ALPHAREF, 0x7f);
		SetCachedRenderState(D3DRS_ALPHAFUNC, 5);
		SetCachedRenderState(D3DRS_STENCILMASK, 0xffffffff);
		SetCachedRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
		SetCachedRenderState(D3DRS_CLIPPING, 1);
		SetCachedRenderState(D3DRS_LIGHTING, 1);
		SetCachedRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);
		SetCachedRenderState(D3DRS_STENCILENABLE, 0);
		SetCachedRenderState(D3DRS_STENCILFAIL, 1);
		SetCachedRenderState(D3DRS_STENCILZFAIL, 1);
		SetCachedRenderState(D3DRS_STENCILPASS, 1);
		SetCachedRenderState(D3DRS_STENCILFUNC, 8);
		SetCachedRenderState(D3DRS_STENCILREF, 0);
		SetCachedRenderState(D3DRS_LOCALVIEWER, 1);
		SetCachedRenderState(D3DRS_NORMALIZENORMALS, 0);
		SetCachedRenderState(D3DRS_VERTEXBLEND, 0);
		SetCachedRenderState(D3DRS_CLIPPLANEENABLE, 0);
		SetCachedRenderState(D3DRS_AMBIENT, m_ambientColor);
		if (m_presentationParameters.MultiSampleType != 0) {
			SetCachedRenderState(D3DRS_MULTISAMPLEANTIALIAS, 1);
		}

		SetTextureFiltering(1);
		m_vtable->m_setDepthState(this, NULL, 1);
		m_vtable->m_action0xc8(this, NULL, 0);
	}

	ResetTextureStages();
	m_vtable->m_action0x124(this, NULL, 0);
}

// STUB: LEGORACERS2 0x004dc590
void DisplayRenderer::SetTextureFiltering(LegoU32 p_linear)
{
	if (p_linear == 1) {
		for (LegoU32 stage = 0; stage < 8; ++stage) {
			if ((m_selectedAdapter->m_caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR) != 0) {
				SetCachedTextureStageState(stage, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
			}
			if ((m_selectedAdapter->m_caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR) != 0) {
				SetCachedTextureStageState(stage, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
			}
		}
	}
	else {
		for (LegoU32 stage = 0; stage < 8; ++stage) {
			if ((m_selectedAdapter->m_caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT) != 0) {
				SetCachedTextureStageState(stage, D3DTSS_MAGFILTER, D3DTEXF_POINT);
			}
			if ((m_selectedAdapter->m_caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT) != 0) {
				SetCachedTextureStageState(stage, D3DTSS_MINFILTER, D3DTEXF_POINT);
			}
		}
	}
}
