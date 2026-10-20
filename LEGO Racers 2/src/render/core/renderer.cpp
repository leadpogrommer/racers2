#include "render/core/renderer.h"

#include <string.h>

// GLOBAL: LEGORACERS2 0x00653078
HRESULT g_lastDirect3DResult;

// STUB: LEGORACERS2 0x004ad2c0
LegoBool32 DisplayRenderer::EnumerateSupportedAdapters()
{
	LegoU32 adapterCount;
	LegoS32 adapterIndex;

	adapterCount = m_direct3D->GetAdapterCount();
	m_adapterCount = 0;
	if ((LegoS32) adapterCount <= 0) {
		return TRUE;
	}

	for (adapterIndex = 0; adapterIndex < (LegoS32) adapterCount; adapterIndex++) {
		D3DCAPS8 caps;
		D3DADAPTER_IDENTIFIER8 identifier;
		RendererAdapterRecord* record;

		memset(&caps, 0, sizeof(caps));
		memset(&identifier, 0, sizeof(identifier));

		g_lastDirect3DResult = m_direct3D->GetDeviceCaps(adapterIndex, D3DDEVTYPE_HAL, &caps);
		if (g_lastDirect3DResult != D3D_OK) {
			continue;
		}

		g_lastDirect3DResult = m_direct3D->GetAdapterIdentifier(adapterIndex, D3DENUM_NO_WHQL_LEVEL, &identifier);
		if (g_lastDirect3DResult != D3D_OK) {
			continue;
		}

		if (m_adapterCount >= sizeOfArray(m_adapters) || caps.DeviceType != D3DDEVTYPE_HAL ||
			(caps.DevCaps & (D3DDEVCAPS_HWRASTERIZATION | D3DDEVCAPS_DRAWPRIMTLVERTEX)) !=
				(D3DDEVCAPS_HWRASTERIZATION | D3DDEVCAPS_DRAWPRIMTLVERTEX) ||
			(caps.ShadeCaps & 0x4008) != 0x4008 || (caps.TextureOpCaps & 0x44) != 0x44 ||
			(caps.PrimitiveMiscCaps & 0x70) != 0x70 || (caps.ZCmpCaps & 0x88) != 0x88 ||
			(caps.SrcBlendCaps & 0x13) != 0x13 || (caps.DestBlendCaps & 0x2f) != 0x2f ||
			(caps.AlphaCmpCaps & 0x88) != 0x88 || (caps.TextureCaps & 0x4005) != 0x4005 ||
			(caps.TextureAddressCaps & 0x5) != 0x5) {
			continue;
		}

		record = &m_adapters[m_adapterCount];
		record->m_adapterIndex = adapterIndex;
		strcpy(record->m_driverName, identifier.Driver);
		strcpy(record->m_description, identifier.Description);
		memcpy(&record->m_caps, &caps, sizeof(caps));
		m_adapterCount++;
	}

	return TRUE;
}
// STUB: LEGORACERS2 0x004ad540
LegoU32 __stdcall GetDisplayFormatBitDepth(D3DFORMAT p_format)
{
	LegoU32 formatIndex = (LegoU32) p_format - 0x14;
	if (formatIndex > 10) {
		return 0;
	}

	switch (formatIndex) {
	case 0:
		return 24;
	case 1:
	case 2:
		return 32;
	case 8:
	case 9:
		return 0;
	default:
		return 16;
	}
}
// FUNCTION: LEGORACERS2 0x004ad5a0
LegoU32 DisplayRenderer::GetSelectedAdapterModeCount()
{
	return m_direct3D->GetAdapterModeCount(m_selectedAdapter->m_adapterIndex);
}
// STUB: LEGORACERS2 0x004ad5c0
void DisplayRenderer::GetSelectedAdapterMode(
	LegoU32 p_modeIndex,
	LegoU32* p_width,
	LegoU32* p_height,
	LegoU32* p_bitDepth,
	LegoU32* p_refreshRate
)
{
	D3DDISPLAYMODE mode;
	memset(&mode, 0, sizeof(mode));
	m_direct3D->EnumAdapterModes(m_selectedAdapter->m_adapterIndex, p_modeIndex, &mode);
	*p_width = mode.Width;
	*p_height = mode.Height;
	*p_refreshRate = mode.RefreshRate;
	*p_bitDepth = GetDisplayFormatBitDepth(mode.Format);
}
// STUB: LEGORACERS2 0x004ada60
LegoBool32 DisplayRenderer::FindBestDisplayMode(
	LegoU32* p_width,
	LegoU32* p_height,
	LegoU32* p_bitDepth,
	LegoU32* p_refreshRate
)
{
	LegoU32 modeCount = GetSelectedAdapterModeCount();
	LegoU32 bestScore = 0x7fffffff;
	LegoU32 bestWidth = 0;
	LegoU32 bestHeight = 0;
	LegoU32 bestRefreshRate = 0;
	D3DFORMAT bestFormat = D3DFMT_UNKNOWN;
	LegoS32 modeIndex;

	if ((LegoS32) modeCount <= 0) {
		return FALSE;
	}

	for (modeIndex = 0; modeIndex < (LegoS32) modeCount; modeIndex++) {
		D3DDISPLAYMODE mode;
		LegoU32 score;

		memset(&mode, 0, sizeof(mode));
		m_direct3D->EnumAdapterModes(m_selectedAdapter->m_adapterIndex, modeIndex, &mode);
		if (GetDisplayFormatBitDepth(mode.Format) != *p_bitDepth || (LegoS32) mode.Width > (LegoS32) *p_width ||
			(LegoS32) mode.Height > (LegoS32) *p_height ||
			(*p_refreshRate != 0 && mode.RefreshRate != *p_refreshRate)) {
			continue;
		}

		score = (*p_width - mode.Width) + (*p_height - mode.Height);
		if ((LegoS32) score >= (LegoS32) bestScore) {
			continue;
		}

		bestScore = score;
		bestWidth = mode.Width;
		bestHeight = mode.Height;
		bestRefreshRate = mode.RefreshRate;
		bestFormat = mode.Format;
	}

	if (bestScore == 0x7fffffff) {
		return FALSE;
	}

	*p_width = bestWidth;
	*p_height = bestHeight;
	m_requestedBitDepth = *p_bitDepth;
	m_modeWidth = bestWidth;
	m_modeHeight = bestHeight;
	m_modeRefreshRate = bestRefreshRate;
	m_modeFormat = bestFormat;
	return TRUE;
}
