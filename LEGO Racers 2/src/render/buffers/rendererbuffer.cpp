#include "render/buffers/rendererbuffer.h"

#include "memory/memoryheap.h"

#include <string.h>

// STUB: LEGORACERS2 0x004bd5f0
void RendererLookupTableState::Initialize(
	LegoU32 p_bucketCount,
	LegoU32 p_entryCapacity,
	LegoU32 p_bucketShift,
	LegoU32 p_unk0x10
)
{
	m_entryCapacity = p_entryCapacity;
	m_bucketCount = p_bucketCount;
	m_bucketShift = p_bucketShift;
	m_unk0x10 = p_unk0x10;

	LegoU32 entryStorageBytes = p_entryCapacity << 4;
	m_entryStorage = (Entry*) AllocateHeapMemory(entryStorageBytes);

	LegoU32 bucketArrayBytes = (p_bucketCount << 2) + 4;
	m_bucketTails = (Entry**) GetMemoryHeap()->Allocate(bucketArrayBytes, "Render group cache order table");
	m_bucketHeads = (Entry**) GetMemoryHeap()->Allocate(bucketArrayBytes, "Render group cache lookup table");

	memset(m_bucketTails, 0, bucketArrayBytes);
	memset(m_bucketHeads, 0, bucketArrayBytes);

	m_itemCount = 1;
	Clear();
}

// STUB: LEGORACERS2 0x004bd6d0
void RendererLookupTableState::ReleaseStorage()
{
	GetMemoryHeap()->Free(m_bucketHeads);
	GetMemoryHeap()->Free(m_bucketTails);
	FreeHeapMemory(m_entryStorage);
}

// STUB: LEGORACERS2 0x004bd710
void RendererLookupTableState::Clear()
{
	if (m_itemCount > 0) {
		memset(m_bucketHeads, 0, m_bucketCount * sizeof(LegoU32));
		m_itemCount = 0;
		m_bucketTailCount = 0;
		*m_bucketTails = 0;
	}
}

// STUB: LEGORACERS2 0x004bd770
void RendererLookupTableState::Insert(LegoU32 p_key, LegoU32 p_unk0x0c, LegoU32 p_unk0x08, LegoS32 p_hashKey)
{
	Entry* entry = &m_entryStorage[m_itemCount];
	++m_itemCount;

	LegoS32 bucketIndex = p_hashKey >> m_bucketShift;
	entry->m_key = p_key;
	entry->m_next = m_bucketHeads[bucketIndex];
	entry->m_unk0x08 = p_unk0x08;
	entry->m_unk0x0c = p_unk0x0c;
	m_bucketHeads[bucketIndex] = entry;
}
