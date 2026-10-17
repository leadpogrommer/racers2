#ifndef LR2_RENDERERBUFFER_H
#define LR2_RENDERERBUFFER_H

#include "decomp.h"
#include "types.h"

#include <stddef.h>

// Observed prefix of the renderer-owned lookup-table state. The renderer's
// constructor installs its vtable separately; this projection does not create
// the table or claim its complete size.
class RendererLookupTableState {
public:
	RendererLookupTableState();
	struct Entry {
		LegoU32 m_key;
		Entry* m_next;
		LegoU32 m_unk0x08;
		LegoU32 m_unk0x0c;
	};

	void Initialize(LegoU32 p_bucketCount, LegoU32 p_entryCapacity, LegoU32 p_bucketShift, LegoU32 p_unk0x10);
	void Insert(LegoU32 p_key, LegoU32 p_unk0x0c, LegoU32 p_unk0x08, LegoS32 p_hashKey);
	void InsertLinear(LegoU32 p_key, LegoU32 p_unk0x0c, LegoU32 p_unk0x08, LegoS32 p_ignoredHash);
	void InsertOrdered(LegoU32 p_key, LegoU32 p_unk0x0c, LegoU32 p_unk0x08, LegoS32 p_hashKey);
	void ReleaseStorage();
	void Clear();
	void FlushBucketedEntries();
	void FlushOrderedEntries();
	void FlushDeferredDrawResources();

	const void* m_vtable;      // 0x00
	Entry** m_bucketHeads;     // 0x04
	Entry** m_bucketTails;     // 0x08
	Entry* m_entryStorage;     // 0x0c
	LegoU32 m_unk0x10;         // 0x10
	LegoU32 m_bucketShift;     // 0x14
	LegoU32 m_entryCapacity;   // 0x18
	LegoU32 m_bucketCount;     // 0x1c
	LegoS32 m_itemCount;       // 0x20
	LegoU32 m_bucketTailCount; // 0x24
};

DECOMP_SIZE_ASSERT(RendererLookupTableState::Entry, 0x10);
DECOMP_SIZE_ASSERT(RendererLookupTableState, 0x28);
DECOMP_STATIC_ASSERT(offsetof(RendererLookupTableState, m_vtable) == 0x00);
DECOMP_STATIC_ASSERT(offsetof(RendererLookupTableState, m_bucketHeads) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(RendererLookupTableState, m_bucketTails) == 0x08);
DECOMP_STATIC_ASSERT(offsetof(RendererLookupTableState, m_entryStorage) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(RendererLookupTableState, m_unk0x10) == 0x10);
DECOMP_STATIC_ASSERT(offsetof(RendererLookupTableState, m_bucketShift) == 0x14);
DECOMP_STATIC_ASSERT(offsetof(RendererLookupTableState, m_entryCapacity) == 0x18);
DECOMP_STATIC_ASSERT(offsetof(RendererLookupTableState, m_bucketCount) == 0x1c);
DECOMP_STATIC_ASSERT(offsetof(RendererLookupTableState, m_itemCount) == 0x20);
DECOMP_STATIC_ASSERT(offsetof(RendererLookupTableState, m_bucketTailCount) == 0x24);

#endif // LR2_RENDERERBUFFER_H
