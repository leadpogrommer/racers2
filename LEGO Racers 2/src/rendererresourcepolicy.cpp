#include "renderer.h"

// FUNCTION: LEGORACERS2 0x004db900
void DisplayRenderer::SetTextureIndexMap(LegoU32 p_index, LegoU32 p_value)
{
	m_textureIndexMap[p_index] = p_value;
}

// FUNCTION: LEGORACERS2 0x004dc4c0
LegoBool32 DisplayRenderer::ShouldUseHighResolutionTextures()
{
	if ((LegoS32) m_totalPhysicalMemory < 0x09600000) {
		return FALSE;
	}
	if (m_unk0x4d8 >= 4) {
		return FALSE;
	}
	if (m_vtable->m_textureQuality(this, NULL) > 0) {
		return FALSE;
	}
	return (LegoS32) m_deviceMemory >= 0x01000000 ? TRUE : FALSE;
}

// STUB: LEGORACERS2 0x004e1f20
LegoBool32 DisplayRenderer::ShouldUseTextureResourceIndex(LegoU32 p_kind)
{
	if (p_kind == 2) {
		return TRUE;
	}
	return p_kind == 3 && !SupportsDotProduct3() ? TRUE : FALSE;
}

// STUB: LEGORACERS2 0x004e1f50
LegoBool32 DisplayRenderer::IsTextureResourceKindAccepted(LegoU32 p_kind)
{
	if (p_kind == 1 || p_kind == 9) {
		return FALSE;
	}
	return p_kind != 3 || !SupportsDotProduct3() ? TRUE : FALSE;
}

// STUB: LEGORACERS2 0x004e1f80
LegoBool32 DisplayRenderer::IsTextureResourceKindSpecial(LegoU32 p_kind)
{
	return p_kind == 3 ? TRUE : FALSE;
}

// STUB: LEGORACERS2 0x004e6380
LegoBool32 DisplayRenderer::SupportsDotProduct3()
{
	return (m_selectedAdapter->m_caps.TextureOpCaps & D3DTEXOPCAPS_DOTPRODUCT3) != 0 ? TRUE : FALSE;
}
