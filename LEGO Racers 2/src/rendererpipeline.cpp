#include "renderer.h"

// STUB: LEGORACERS2 0x004dee20
void DisplayRenderer::SetStencilBufferEnabled(LegoU32 p_enabled)
{
	LegoU32 enabled = p_enabled == 1 && m_foundPreferredDepthFormat == 1 ? 1 : 0;
	if (m_cachedRenderStates[D3DRS_STENCILENABLE] != enabled) {
		SetCachedRenderState(D3DRS_STENCILENABLE, enabled);
	}
}

// STUB: LEGORACERS2 0x004dee90
void DisplayRenderer::ResetStencilOperations(LegoU32 p_ignored)
{
	(void) p_ignored;
	if (m_foundPreferredDepthFormat == 1) {
		m_vtable->m_setStencilOperation(this, NULL, 0, 0, 0);
		m_vtable->m_setStencilOperation(this, NULL, 1, 0, 0);
		m_vtable->m_setStencilOperation(this, NULL, 2, 0, 0);
	}
}

// STUB: LEGORACERS2 0x004deed0
void DisplayRenderer::SetStencilFunction(LegoU32 p_function)
{
	if (m_foundPreferredDepthFormat != 1 || p_function > 7) {
		return;
	}

	SetCachedRenderState(D3DRS_STENCILFUNC, D3DCMP_NEVER + p_function);
}

// STUB: LEGORACERS2 0x004deff0
void DisplayRenderer::SetStencilOperation(LegoU32 p_target, LegoU32 p_operation, LegoU32 p_reference)
{
	if (m_foundPreferredDepthFormat != 1 || p_operation > 7) {
		return;
	}

	D3DRENDERSTATETYPE state;
	switch (p_target) {
	case 0:
		state = D3DRS_STENCILFAIL;
		break;
	case 1:
		state = D3DRS_STENCILZFAIL;
		break;
	case 2:
		state = D3DRS_STENCILPASS;
		break;
	default:
		state = (D3DRENDERSTATETYPE) p_target;
		break;
	}

	LegoU32 operation = D3DSTENCILOP_KEEP + p_operation;
	if (m_cachedRenderStates[state] != operation) {
		SetCachedRenderState(state, operation);
	}

	if (p_operation == 2 && m_cachedRenderStates[D3DRS_STENCILREF] != p_reference) {
		SetCachedRenderState(D3DRS_STENCILREF, p_reference);
	}
}

// STUB: LEGORACERS2 0x004df190
void DisplayRenderer::SetColorOverride(LegoU32 p_color)
{
	const LegoFloat byteScale = 1.0f / 255.0f;
	m_colorOverrideEnabled = 1;
	m_colorOverride[0] = (LegoFloat) (p_color & 0xff) * byteScale;
	m_colorOverride[1] = (LegoFloat) ((p_color >> 8) & 0xff) * byteScale;
	m_colorOverride[2] = (LegoFloat) ((p_color >> 16) & 0xff) * byteScale;
	m_colorOverride[3] = (LegoFloat) ((p_color >> 24) & 0xff) * byteScale;
}

// FUNCTION: LEGORACERS2 0x004df220
void DisplayRenderer::DisableColorOverride()
{
	m_colorOverrideEnabled = 0;
}

// FUNCTION: LEGORACERS2 0x004df230
void DisplayRenderer::SetBlendModeOverride(LegoU32 p_mode)
{
	m_useBlendModeOverride = 1;
	m_blendModeOverride = p_mode;
}

// FUNCTION: LEGORACERS2 0x004df250
void DisplayRenderer::ClearBlendModeOverride()
{
	m_useBlendModeOverride = 0;
}
