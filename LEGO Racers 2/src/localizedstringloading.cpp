#include "charbuffer24.h"
#include "localizedstringtable.h"
#include "memoryheap.h"
#include "winfile.h"

#include <stdio.h>
#include <string.h>

// FUNCTION: LEGORACERS2 0x004b7550
LocalizedStringTable::LocalizedStringTable() : m_suffixes(NULL), m_languageCount(0), m_path()
{
	m_bufferSize = 0;
	m_buffer = NULL;
	m_stringCount = 0;
	m_stringOffsets = NULL;
	m_stringData = NULL;
	m_order = NULL;
	CharBuffer24 fallback("Not A String");
	const LegoChar* input = fallback.m_data;
	LegoU16* output = m_emptyString;
	do {
		*output++ = (LegoS16) *input;
	} while (*input++ != 0);
}

// STUB: LEGORACERS2 0x004b75c0
void LocalizedStringTable::Load(const CharBuffer128* p_path, const LegoChar* p_suffixes, LegoS32 p_count)
{
	m_path = *p_path;
	m_languageCount = p_count;
	m_suffixes = (LegoChar*) GetMemoryHeap()->Allocate(p_count * 4, "Localisation - File extension array space");
	for (LegoS32 i = 0; i < p_count; ++i) {
		strcpy(m_suffixes + i * 4, p_suffixes + i * 4);
	}
	m_bufferSize = 0;
	for (i = 0; i < p_count; ++i) {
		BuildLanguagePath(i);
		WinFile file(m_path.m_data, 0, NULL);
		LegoS32 size = file.GetManagedSize();
		if (m_bufferSize < size) {
			m_bufferSize = size;
		}
	}
	m_buffer = (LegoU8*) GetMemoryHeap()->Allocate(m_bufferSize, "Localisation strings buffer");
	BuildLanguagePath(0);
	WinFile file(m_path.m_data, 0, NULL);
	LegoU32 headerCount;
	file.Read(&headerCount, 4, 1);
	LegoU32 tableEnd = headerCount * 4 + 8;
	file.Read(m_buffer, tableEnd + 4, 1);
	file.CloseManaged();
	LegoU32 stringCount = *(LegoU32*) (m_buffer + tableEnd);
	m_order =
		(LocalizedStringOrderEntry*) GetMemoryHeap()->Allocate(stringCount * 8, "Localisation strings order lookup");
}

// STUB: LEGORACERS2 0x004b7740
void LocalizedStringTable::Release()
{
	if (m_order != NULL) {
		GetMemoryHeap()->Free(m_order);
		m_order = NULL;
	}
	if (m_buffer != NULL) {
		GetMemoryHeap()->Free(m_buffer);
		m_buffer = NULL;
	}
	if (m_suffixes != NULL) {
		GetMemoryHeap()->Free(m_suffixes);
		m_suffixes = NULL;
	}
	m_languageCount = 0;
	m_stringCount = 0;
}

// STUB: LEGORACERS2 0x004b77b0
void LocalizedStringTable::SelectLanguage(LegoS32 p_language, const void* p_source, LegoU32 p_stride)
{
	m_offsetStride = p_stride;
	if (p_source == NULL) {
		BuildLanguagePath(p_language);
		WinFile file(m_path.m_data, 0, NULL);
		LegoU32 size = file.GetManagedSize();
		file.Read(m_buffer, size, 1);
		file.CloseManaged();
	}
	else {
		const LegoU32* input = (const LegoU32*) p_source;
		LegoU32* output = (LegoU32*) m_buffer;
		for (LegoS32 i = 0; i < (m_bufferSize + 3) >> 2; ++i) {
			*output++ = *input++;
		}
	}
	LegoU32* header = (LegoU32*) m_buffer;
	m_stringCount = header[header[0] + 3];
	LegoU8* entry = (LegoU8*) (header + header[0] + 4);
	m_stringOffsets = (LegoU32*) entry;
	for (LegoS32 i = 0; i < m_stringCount; ++i) {
		LegoS32* offset = (LegoS32*) (entry + m_offsetStride - 4);
		*offset -= (m_stringCount - i) * m_offsetStride;
		*offset = (*offset + m_offsetStride - 4) >> 1;
		entry = (LegoU8*) (offset + 1);
	}
	m_stringData = (const LegoU16*) entry;
	BuildSortedIndex();
}

// FUNCTION: LEGORACERS2 0x004b79c0
void LocalizedStringTable::BuildLanguagePath(LegoS32 p_language)
{
	LegoChar* extension = m_path.FindAfterDot();
	if (extension != NULL) {
		strcpy(extension, m_suffixes + p_language * 4);
	}
	else {
		LegoChar suffix[12];
		sprintf(suffix, ".%s", m_suffixes + p_language * 4);
		strcat(m_path.m_data, suffix);
	}
}

// STUB: LEGORACERS2 0x004b7a60
void LocalizedStringTable::BuildSortedIndex()
{
	const LegoChar* key = (const LegoChar*) m_stringOffsets;
	for (LegoS32 index = 0; index < m_stringCount; ++index) {
		LegoS32 lower = 0;
		LegoS32 upper = index;
		while (lower < upper) {
			LegoS32 middle = lower + ((upper - lower) >> 1);
			LegoS32 order = strcmp(m_order[middle].m_key, key);
			if (order == 0) {
				lower = middle;
				break;
			}
			if (order < 0) {
				lower = middle + 1;
			}
			else {
				upper = middle;
			}
		}
		for (LegoS32 i = index - 1; i >= lower; --i) {
			m_order[i + 1] = m_order[i];
		}
		m_order[lower].m_key = key;
		m_order[lower].m_stringIndex = index;
		key += m_offsetStride;
	}
}
