#include "renderer.h"

// FUNCTION: LEGORACERS2 0x004dfd10
LegoU32 DisplayRenderer::UpdateTextureStageFlags(RendererDefaultSlot* p_slot, LegoU32 p_mode)
{
	LegoU32 flags = 0;
	if (p_slot->m_value == 1 || p_slot->m_value == 9) {
		if (p_slot->m_value == 9 && (m_textureRecords[(LegoS16) p_slot->m_mask].m_flags0x9c & 8) == 8) {
			flags = p_mode == 1 ? 0x10 : 0x30;
		}
		else {
			flags = 0x30;
		}
	}
	p_slot->m_kind = (p_slot->m_kind & 0x0f) | (LegoU8) flags;
	return flags;
}
