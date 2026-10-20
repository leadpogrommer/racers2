#include "foyeranimationmaster.h"

#include "memoryheap.h"
#include "winfile.h"

#include <new>
#include <string.h>

// GLOBAL: LEGORACERS2 0x006e4e98
static LegoS32 g_foyerAnimationMasterCacheReferences;
// GLOBAL: LEGORACERS2 0x006e4e9c
static FoyerAnimationMasterCache* g_foyerAnimationMasterCache;

// STUB: LEGORACERS2 0x004f63c0
FoyerAnimationMasterCache* AcquireFoyerAnimationMasterCache()
{
	if (g_foyerAnimationMasterCacheReferences == 0) {
		void* storage = AllocateHeapMemory(sizeof(FoyerAnimationMasterCache));
		g_foyerAnimationMasterCache = storage == NULL ? NULL : new (storage) FoyerAnimationMasterCache;
	}
	++g_foyerAnimationMasterCacheReferences;
	return g_foyerAnimationMasterCache;
}

// STUB: LEGORACERS2 0x004f6410
void ReleaseFoyerAnimationMasterCache()
{
	FoyerAnimationMasterCache* cache = g_foyerAnimationMasterCache;
	--g_foyerAnimationMasterCacheReferences;
	if (g_foyerAnimationMasterCacheReferences == 0 && cache != NULL) {
		cache->~FoyerAnimationMasterCache();
		FreeHeapMemory(cache);
	}
}

// FUNCTION: LEGORACERS2 0x004f6440
FoyerAnimationMasterCache::FoyerAnimationMasterCache()
{
	m_usedCount = 0;
	for (LegoU32 i = 0; i < 128; ++i) {
		m_entries[i].m_used = 0;
		m_entries[i].m_references = 0;
	}
}

// STUB: LEGORACERS2 0x004f6490
FoyerAnimationMasterCache::~FoyerAnimationMasterCache()
{
	for (LegoS32 i = 127; i >= 0; --i) {
		if (m_entries[i].m_used != 0 && m_entries[i].m_references != 0) {
			Release(i);
		}
	}
}

// STUB: LEGORACERS2 0x004f64d0
LegoS32 FoyerAnimationMasterCache::FindOrAdd(
	const LegoChar* p_directory,
	const CharBuffer24* p_descriptor,
	LegoU32 p_retain
)
{
	LegoS32 i;
	for (i = 0; i < 128; ++i) {
		FoyerAnimationMasterEntry* entry = &m_entries[i];
		if (entry->m_used != 0 && strcmp(p_directory, entry->m_directory) == 0 &&
			strncmp(entry->m_descriptor.m_data, p_descriptor->m_data, sizeof(entry->m_descriptor.m_data)) == 0) {
			if (p_retain != 0) {
				Retain(i);
			}
			return i;
		}
	}
	for (i = 0; i < 128; ++i) {
		FoyerAnimationMasterEntry* entry = &m_entries[i];
		if (entry->m_used == 0) {
			entry->m_descriptor = *p_descriptor;
			strcpy(entry->m_directory, p_directory);
			entry->m_used = 1;
			entry->m_references = 0;
			if (p_retain != 0) {
				Retain(i);
			}
			++m_usedCount;
			return i;
		}
	}
	return -1;
}

// STUB: LEGORACERS2 0x004f6610
void FoyerAnimationMasterCache::Retain(LegoU32 p_index)
{
	FoyerAnimationMasterEntry* entry = &m_entries[p_index];
	if (entry->m_references != 0) {
		++entry->m_references;
		return;
	}
	entry->m_references = 1;
	LegoChar path[0x100];
	strcpy(path, entry->m_directory);
	strcat(path, entry->m_descriptor.m_data);
	strcat(path, ".bsa");
	WinFile file(path, 0, NULL);
	LegoU32 length = file.GetManagedSize();
	LegoU8* header = (LegoU8*) GetMemoryHeap()->AllocateFromEnd(0x20, "ANM Header");
	entry->m_body = (LegoU8*) GetMemoryHeap()->Allocate(length - 0x20, "ANM Body");
	file.Read(header, 0x20, 1);
	file.Read(entry->m_body, length - 0x20, 1);
	entry->m_frameCount = *(LegoS16*) (header + 0x10);
	entry->m_flags = *(LegoS16*) (header + 0x12);
	entry->m_quaternionCount = *(LegoU32*) (header + 0x18);
	entry->m_frameRate = *(LegoFloat*) (header + 0x14);
	entry->m_unk0x140 = *(LegoFloat*) (header + 0x1c);
	entry->m_frameCountFloat = (LegoFloat) entry->m_frameCount;
	entry->m_frameStride = entry->m_quaternionCount * sizeof(Quaternion) + 0x18;
	LegoFloat inverse = 1.0f / entry->m_frameRate;
	entry->m_frameSeconds = inverse;
	entry->m_duration = inverse * (LegoFloat) entry->m_frameCount;
	GetMemoryHeap()->Free(header);
}

// STUB: LEGORACERS2 0x004f67e0
void FoyerAnimationMasterCache::Release(LegoU32 p_index)
{
	FoyerAnimationMasterEntry* entry = &m_entries[p_index];
	--entry->m_references;
	if (entry->m_references == 0) {
		GetMemoryHeap()->Free(entry->m_body);
		entry->m_body = NULL;
	}
}
