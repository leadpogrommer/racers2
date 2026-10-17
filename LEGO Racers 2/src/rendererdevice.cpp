#include "renderer.h"

namespace
{
// Retail order from 0x00549e00 and 0x00549e1c. The alternate ordering is used
// when the requested display depth exceeds 16 bits.
// GLOBAL: LEGORACERS2 0x00549e00
const D3DFORMAT k_depthFormats16BitFirst[] =
	{D3DFMT_D16, D3DFMT_D15S1, D3DFMT_D24S8, D3DFMT_D24X4S4, D3DFMT_D32, D3DFMT_D24X8};
// GLOBAL: LEGORACERS2 0x00549e1c
const D3DFORMAT k_depthFormatsHighBitFirst[] =
	{D3DFMT_D24S8, D3DFMT_D24X4S4, D3DFMT_D32, D3DFMT_D24X8, D3DFMT_D16, D3DFMT_D15S1};
} // namespace

// STUB: LEGORACERS2 0x004aebb0
LegoBool32 DisplayRenderer::SelectDepthStencilFormat()
{
	const D3DFORMAT* formats = k_depthFormats16BitFirst;
	LegoU32 formatCount = sizeof(k_depthFormats16BitFirst) / sizeof(k_depthFormats16BitFirst[0]);
	if ((LegoS32) m_requestedBitDepth > 16) {
		formats = k_depthFormatsHighBitFirst;
		formatCount = sizeof(k_depthFormatsHighBitFirst) / sizeof(k_depthFormatsHighBitFirst[0]);
	}

	m_foundPreferredDepthFormat = FALSE;
	m_unk0x24574 = FALSE;

	for (LegoU32 index = 0; index < formatCount; index++) {
		D3DFORMAT format = formats[index];
		HRESULT result = m_direct3D->CheckDeviceFormat(
			m_selectedAdapter->m_adapterIndex,
			D3DDEVTYPE_HAL,
			m_modeFormat,
			D3DUSAGE_DEPTHSTENCIL,
			D3DRTYPE_SURFACE,
			format
		);
		if (result == D3D_OK) {
			if (format == D3DFMT_D24S8) {
				m_foundPreferredDepthFormat = TRUE;
			}
			m_depthStencilFormat = format;
			return TRUE;
		}
	}

	return FALSE;
}
