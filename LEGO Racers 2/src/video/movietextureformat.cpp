#include "render/core/renderer.h"

// FUNCTION: LEGORACERS2 0x004dc210
LegoBool32 DisplayRenderer::IsTextureFormatSupported(D3DFORMAT p_format)
{
	return m_direct3D->CheckDeviceFormat(
			   m_selectedAdapter->m_adapterIndex,
			   D3DDEVTYPE_HAL,
			   m_modeFormat,
			   0,
			   D3DRTYPE_TEXTURE,
			   p_format
		   ) == D3D_OK;
}
