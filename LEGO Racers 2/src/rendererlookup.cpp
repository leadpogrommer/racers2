#include "rendererbuffer.h"

namespace
{

struct RendererLookupDispatchView {
	typedef void(__fastcall* Flush)(RendererLookupTableState*, void*);
	void* m_insert;
	Flush m_flush;
};

} // namespace

// FUNCTION: LEGORACERS2 0x004bd7c0
void RendererLookupTableState::InsertOrdered(LegoU32 p_key, LegoU32 p_unk0x0c, LegoU32 p_unk0x08, LegoS32 p_hashKey)
{
	if (m_itemCount >= (LegoS32) m_entryCapacity) {
		RendererLookupDispatchView* dispatch = (RendererLookupDispatchView*) m_vtable;
		dispatch->m_flush(this, m_vtable);
	}

	Entry* entry = &m_entryStorage[m_itemCount];
	++m_itemCount;

	Entry** bucketHead = &m_bucketHeads[p_hashKey + 1];
	Entry* bucketTail = *bucketHead;
	if (bucketTail == NULL) {
		bucketTail = (Entry*) &m_bucketTails[m_bucketTailCount++];
		bucketTail->m_next = NULL;
		*bucketHead = bucketTail;
	}

	entry->m_key = p_key;
	entry->m_next = (Entry*) bucketTail->m_key;
	entry->m_unk0x08 = p_unk0x08;
	entry->m_unk0x0c = p_unk0x0c;
	bucketTail->m_key = (LegoU32) entry;
}
