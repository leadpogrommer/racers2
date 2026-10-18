#ifndef LR2_LOCALIZEDSTRINGTABLE_H
#define LR2_LOCALIZEDSTRINGTABLE_H

#include "charbuffer128.h"
#include "decomp.h"
#include "types.h"

#include <stddef.h>

struct LocalizedStringOrderEntry {
	const LegoChar* m_key;
	LegoS32 m_stringIndex;
};
DECOMP_SIZE_ASSERT(LocalizedStringOrderEntry, 8);

struct LocalizedStringTable {
	LocalizedStringTable();
	void Load(const CharBuffer128* p_path, const LegoChar* p_suffixes, LegoS32 p_count);
	void Release();
	void SelectLanguage(LegoS32 p_language, const void* p_source, LegoU32 p_stride);
	void BuildLanguagePath(LegoS32 p_language);
	void BuildSortedIndex();
	LegoChar* m_suffixes;
	LegoS32 m_languageCount;
	CharBuffer128 m_path;
	LegoS32 m_bufferSize;
	LegoU8* m_buffer;
	LegoS32 m_stringCount;
	LegoU32* m_stringOffsets;
	LegoU32 m_offsetStride;
	const LegoU16* m_stringData;
	LegoU16 m_emptyString[24];
	LocalizedStringOrderEntry* m_order;

	LegoBool32 GetString(LegoS32 p_stringIndex, const LegoU16** p_string) const;
};

DECOMP_STATIC_ASSERT(offsetof(LocalizedStringTable, m_stringCount) == 0x90);
DECOMP_STATIC_ASSERT(offsetof(LocalizedStringTable, m_stringOffsets) == 0x94);
DECOMP_STATIC_ASSERT(offsetof(LocalizedStringTable, m_offsetStride) == 0x98);
DECOMP_STATIC_ASSERT(offsetof(LocalizedStringTable, m_stringData) == 0x9c);
DECOMP_STATIC_ASSERT(offsetof(LocalizedStringTable, m_emptyString) == 0xa0);

DECOMP_SIZE_ASSERT(LocalizedStringTable, 0xd4);

LocalizedStringTable* GetLocalizedStringTable();

#endif // LR2_LOCALIZEDSTRINGTABLE_H
