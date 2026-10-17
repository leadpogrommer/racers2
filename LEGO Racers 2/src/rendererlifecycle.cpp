#include "rendererrecordgroup.h"
#include "rendererzeropair.h"

#include <string.h>

// FUNCTION: LEGORACERS2 0x004abe90
RendererZeroPair::RendererZeroPair()
{
	m_first = 0;
	m_second = 0;
}

// FUNCTION: LEGORACERS2 0x004abeb0
RendererSubrecordGroup::RendererSubrecordGroup()
{
	for (LegoU32 index = 0; index < sizeOfArray(m_records); index++) {
		Subrecord* record = &m_records[index];
		record->m_unk0x20 = 0;
		record->m_unk0x2c = 1;
		record->m_unk0x28 = 0;
		memset(record->m_unk0x00, 0xff, sizeof(record->m_unk0x00));
	}
	m_unk0xc0 = 0;
}
