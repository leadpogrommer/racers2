#include "bitmapfont.h"
#include "globaloverlaymanager.h"

// STUB: LEGORACERS2 0x004b3dc0
void GlobalOverlayRecord::Translate(const LegoFloat* p_offset)
{
	m_positionX += p_offset[0];
	m_positionY = p_offset[1] + m_positionY;
}

// FUNCTION: LEGORACERS2 0x004b3df0
void GlobalOverlayRecord::SetColor(const LegoU32* p_color)
{
	m_color = *p_color;
}

// FUNCTION: LEGORACERS2 0x004b3e00
void GlobalOverlayRecord::SetAlpha(LegoU8 p_alpha)
{
	((LegoU8*) &m_color)[3] = p_alpha;
}

// STUB: LEGORACERS2 0x004b3e10
void GlobalOverlayRecord::SetList(LegoS32 p_listId)
{
	g_globalOverlayManager.MoveRecord(this, m_listId, p_listId);
	m_listId = p_listId;
}

// FUNCTION: LEGORACERS2 0x004b3ea0
LegoFloat GlobalOverlayRecord::GetEffectiveScale() const
{
	if ((m_animationFlags & 8) != 0) {
		return m_scale;
	}
	return 1.0f;
}

// FUNCTION: LEGORACERS2 0x004b3ec0
LegoFloat GlobalOverlayRecord::GetLineHeight() const
{
	BitmapFont* font = m_font;
	LegoFloat scale = GetEffectiveScale();
	return font->m_unk0x20 * font->m_scaleY * scale;
}

// FUNCTION: LEGORACERS2 0x004b3ee0
void GlobalOverlayRecord::SetClipRectangle(const LegoFloat* p_topLeft, const LegoFloat* p_bottomRight)
{
	m_clipLeft = p_topLeft[0];
	m_clipTop = p_topLeft[1];
	m_clipRight = p_bottomRight[0];
	m_clipBottom = p_bottomRight[1];
}

// FUNCTION: LEGORACERS2 0x004b3f10
LegoS32 GlobalOverlayRecord::GetTextLength() const
{
	LegoS32 length = 0;
	while (length < 256 && m_text[length] != 0) {
		++length;
	}
	return length;
}
