#include "renderer.h"

// FUNCTION: LEGORACERS2 0x004abef0
LegoFloat* DisplayRenderer::GetSelectedLightDirection()
{
	return m_lightDirection;
}

// FUNCTION: LEGORACERS2 0x004ac030
LegoBool32 DisplayRenderer::HasPreferredDepthFormat()
{
	return m_foundPreferredDepthFormat;
}

// FUNCTION: LEGORACERS2 0x004ac060
LegoFloat DisplayRenderer::GetStageBlend()
{
	return m_stageBlend;
}

// FUNCTION: LEGORACERS2 0x004ac070
void DisplayRenderer::SetStageBlend(LegoFloat p_blend)
{
	m_stageBlend = p_blend;
}

// STUB: LEGORACERS2 0x004ac0a0
LegoU32 DisplayRenderer::GetMaximumTextureDimension()
{
	LegoU32 width = m_selectedAdapter->m_caps.MaxTextureWidth;
	LegoU32 height = m_selectedAdapter->m_caps.MaxTextureHeight;
	LegoU32 maximum = width < height ? width : height;
	return maximum < 1024 ? maximum : 1024;
}

// FUNCTION: LEGORACERS2 0x004ac0d0
RendererVertexBuffer* DisplayRenderer::GetQuadVertexBuffer()
{
	return m_quadVertexBuffer;
}

// FUNCTION: LEGORACERS2 0x004ac0f0
void DisplayRenderer::SetClockwiseCulling(LegoBool32 p_enabled)
{
	m_clockwiseCulling = p_enabled;
}

// FUNCTION: LEGORACERS2 0x004ac100
void DisplayRenderer::SetUnknownDrawPolicy(LegoU32 p_value)
{
	m_unk0x810 = p_value;
}

// STUB: LEGORACERS2 0x004aeb50
LegoU32 DisplayRenderer::GetDotProductStageFlag(LegoU32 p_flags)
{
	if ((p_flags & 8) != 0 && SupportsDotProduct3() == 1) {
		return 0x10;
	}
	return 0;
}

// STUB: LEGORACERS2 0x004aeb70
LegoBool32 DisplayRenderer::SupportsExtendedTextureStages()
{
	if (m_vtable->m_usesHardwareVertexProcessing(this, NULL) && m_vtable->m_hasPreferredDepthFormat(this, NULL) == 1 &&
		m_unk0x4d0 > 9 && m_unk0x4d4 > 8) {
		return TRUE;
	}
	return FALSE;
}

// FUNCTION: LEGORACERS2 0x004aec60
LegoS32 DisplayRenderer::GetDeviceMemoryMegabytes()
{
	return (LegoS32) m_deviceMemory >> 20;
}
