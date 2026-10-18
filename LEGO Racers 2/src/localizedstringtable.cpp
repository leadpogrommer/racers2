#include "localizedstringtable.h"

// GLOBAL: LEGORACERS2 0x00679b70
LocalizedStringTable* g_localizedStringTable = NULL;

// FUNCTION: LEGORACERS2 0x004b78c0
LegoBool32 LocalizedStringTable::GetString(LegoS32 p_stringIndex, const LegoU16** p_string) const
{
	if (p_stringIndex >= 0 && p_stringIndex < m_stringCount) {
		const LegoU8* offsets = (const LegoU8*) m_stringOffsets;
		LegoU32 dataOffset = *(const LegoU32*) (offsets + m_offsetStride * (p_stringIndex + 1) - 4);
		*p_string = m_stringData + dataOffset;
	}
	else {
		*p_string = m_emptyString;
	}
	return 1;
}
