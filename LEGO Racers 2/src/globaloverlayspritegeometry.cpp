#include "globaloverlaymanager.h"

// STUB: LEGORACERS2 0x004b2e40
void GlobalOverlaySprite::SetPosition(const LegoFloat* p_position)
{
	LegoFloat width = m_right - m_left;
	LegoFloat height = m_bottom - m_top;
	m_left = p_position[0];
	m_top = p_position[1];
	m_right = p_position[0];
	m_bottom = p_position[1];
	if ((m_alignment & 1) != 0) {
		LegoFloat halfWidth = width * 0.5f;
		LegoFloat halfHeight = height * 0.5f;
		m_left -= halfWidth;
		m_top -= halfHeight;
		m_right += halfWidth;
		m_bottom += halfHeight;
	}
	else {
		m_right += width;
		m_bottom += height;
	}
}

// STUB: LEGORACERS2 0x004b2f00
void GlobalOverlaySprite::Translate(const LegoFloat* p_offset)
{
	m_left += p_offset[0];
	m_top += p_offset[1];
	m_right += p_offset[0];
	m_bottom += p_offset[1];
}

// FUNCTION: LEGORACERS2 0x004b2f30
void GlobalOverlaySprite::SetSize(const LegoFloat* p_size)
{
	if (p_size[0] >= 0.0f) {
		if ((m_alignment & 1) != 0) {
			LegoFloat halfDifference = (p_size[0] - (m_right - m_left)) * 0.5f;
			m_left -= halfDifference;
			m_right += halfDifference;
		}
		else {
			m_right = m_left + p_size[0];
		}
	}
	if (p_size[1] >= 0.0f) {
		if ((m_alignment & 1) != 0) {
			LegoFloat halfDifference = (p_size[1] - (m_bottom - m_top)) * 0.5f;
			m_top -= halfDifference;
			m_bottom += halfDifference;
		}
		else {
			m_bottom = m_top + p_size[1];
		}
	}
}

// FUNCTION: LEGORACERS2 0x004b2fb0
void GlobalOverlaySprite::GetSize(LegoFloat* p_size) const
{
	p_size[0] = m_right - m_left;
	p_size[1] = m_bottom - m_top;
}

// FUNCTION: LEGORACERS2 0x004b2fd0
void GlobalOverlaySprite::GetRectangle(LegoFloat* p_topLeft, LegoFloat* p_bottomRight) const
{
	p_topLeft[0] = m_left;
	p_topLeft[1] = m_top;
	p_bottomRight[0] = m_right;
	p_bottomRight[1] = m_bottom;
}

// FUNCTION: LEGORACERS2 0x004b3000
void GlobalOverlaySprite::SetTextureRegion(
	const LegoFloat* p_topLeft,
	const LegoFloat* p_bottomRight,
	LegoS32 p_texture
)
{
	if (p_texture != -1) {
		m_texture = p_texture;
	}
	m_u = p_topLeft[0];
	m_v = p_topLeft[1];
	m_uEnd = p_bottomRight[0];
	m_vEnd = p_bottomRight[1];
}

// FUNCTION: LEGORACERS2 0x004b3030
void GlobalOverlaySprite::SetTexture(LegoU32 p_texture)
{
	m_texture = p_texture;
}

// FUNCTION: LEGORACERS2 0x004b3040
void GlobalOverlaySprite::SetColor(const LegoU32* p_color)
{
	m_color = *p_color;
}

// FUNCTION: LEGORACERS2 0x004b3050
void GlobalOverlaySprite::SetAlpha(LegoU8 p_alpha)
{
	((LegoU8*) &m_color)[3] = p_alpha;
}

// FUNCTION: LEGORACERS2 0x004b3060
void GlobalOverlaySprite::SetType(LegoU32 p_type)
{
	m_type = p_type;
}

// STUB: LEGORACERS2 0x004b3070
void GlobalOverlaySprite::SetList(LegoS32 p_listId)
{
	g_globalOverlayManager.MoveSprite(this, m_listId, p_listId);
	m_listId = p_listId;
}

// FUNCTION: LEGORACERS2 0x004b3090
void GlobalOverlaySprite::SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight)
{
	m_clipLeft = p_topLeft[0];
	m_clipTop = p_topLeft[1];
	m_clipRight = p_bottomRight[0];
	m_clipBottom = p_bottomRight[1];
}
