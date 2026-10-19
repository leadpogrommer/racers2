#include "globaloverlaymanager.h"
#include "menulabelselector.h"

// STUB: LEGORACERS2 0x0041bc20
void MenuLabelSelector::Initialize(
	LegoS32 p_initialValue,
	const LegoFloat* p_position,
	const LegoS32* p_stringIndices,
	LegoS32 p_count,
	LegoS32 p_selection,
	const LegoS32* p_numbers,
	LegoBool32 p_alignLeft,
	LegoFloat p_scale,
	LegoFloat p_minimumWidth,
	LegoU32 p_wrap
)
{
	LegoU32 recordParameters[13];
	LegoS32 index;
	LegoFloat position[2];
	LegoFloat width;
	union {
		LegoFloat m_float;
		LegoU32 m_word;
	} scaleBits;

	BeginState();
	m_initialValue = p_initialValue;
	m_unk0x14 = p_wrap;
	m_count = p_count;
	m_selection = p_selection;
	for (index = 0; index < p_count; ++index) {
		m_stringIndices[index] = p_stringIndices[index];
		m_numbers[index] = p_numbers == NULL ? -1 : p_numbers[index];
	}
	scaleBits.m_float = p_scale;
	m_unk0xa4 = scaleBits.m_word;
	m_minimumWidth = p_minimumWidth;

	recordParameters[0] = (LegoU32) m_font;
	recordParameters[1] = 0xffffffff;
	recordParameters[2] = 0;
	recordParameters[3] = 0;
	recordParameters[4] = 0;
	recordParameters[5] = 0;
	recordParameters[6] = 0x3f800000;
	recordParameters[7] = 0x3f800000;
	recordParameters[8] = 0;
	recordParameters[9] = 9;
	recordParameters[10] = 0x3f800000;
	recordParameters[11] = 0;
	recordParameters[12] = 0;
	m_label = g_globalOverlayManager.AllocateRecord(recordParameters, -1);
	m_label->SetScale(p_scale);
	SelectLabel(m_selection);
	m_label->SetPosition(p_position, 0xffffffff);
	m_vtable->m_updateBounds(this, NULL);

	position[0] = p_position[0];
	position[1] = p_position[1];
	width = (m_bounds[2] - m_bounds[0]) * 0.5f;
	if (p_alignLeft == 0) {
		position[0] += width;
		m_label->SetPosition(position, 0xffffffff);
	}
	if (m_count > 1) {
		position[1] += m_label->GetLineHeight() * 0.5f;
		CreateChildSprites(position, width, -1, p_scale);
	}
	m_vtable->m_updateBounds(this, NULL);
}
