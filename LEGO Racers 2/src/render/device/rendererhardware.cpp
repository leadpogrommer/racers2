#include "render/core/renderer.h"
#include "render/device/displayconfig.h"

// STUB: LEGORACERS2 0x004acd00
void DisplayRenderer::ConfigureNvidiaWorkaround()
{
	D3DADAPTER_IDENTIFIER8 identifier;
	m_nvidiaWorkaround = FALSE;
	HRESULT result = m_direct3D->GetAdapterIdentifier(m_requestedAdapterIndex, D3DENUM_NO_WHQL_LEVEL, &identifier);
	if (result == D3D_OK && identifier.VendorId == 0x10de) {
		// Device IDs decoded from both retail switch tables, including the
		// separate 0x103 branch. These identify the game's driver workaround.
		switch (identifier.DeviceId) {
		case 0x20:
		case 0x28:
		case 0x29:
		case 0x2c:
		case 0x2d:
		case 0xa0:
		case 0x100:
		case 0x101:
		case 0x103:
		case 0x110:
		case 0x111:
		case 0x113:
		case 0x150:
		case 0x151:
		case 0x152:
		case 0x153:
		case 0x200:
			m_nvidiaWorkaround = TRUE;
		}
	}

	// Retail clears this state for matching hardware even when the user's
	// NVidia setting subsequently disables the workaround.
	if (m_nvidiaWorkaround == TRUE) {
		m_vertexBufferUsage = 0;
	}
	if (g_displayConfiguration->m_nvidia == 0) {
		m_nvidiaWorkaround = FALSE;
	}
}
