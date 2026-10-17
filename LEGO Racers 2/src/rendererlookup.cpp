#include "rendererbuffer.h"

extern "C" int __cdecl _purecall();

namespace
{

struct RendererLookupDispatchView {
	typedef void(__fastcall* Flush)(RendererLookupTableState*, void*);
	void* m_insert;
	Flush m_flush;
};

// Retail's abstract base table has two genuine CRT purecall entries. Its
// constructor installs it before the derived two-slot table is selected.
struct RendererAbstractLookupDispatch {
	int(__cdecl* m_insert)();
	int(__cdecl* m_flush)();
};
DECOMP_SIZE_ASSERT(RendererAbstractLookupDispatch, 8);

// GLOBAL: LEGORACERS2 0x005358dc
const RendererAbstractLookupDispatch g_abstractRendererLookupDispatch = {_purecall, _purecall};

} // namespace

// FUNCTION: LEGORACERS2 0x004abdb0
RendererLookupTableState::RendererLookupTableState()
{
	m_bucketHeads = NULL;
	m_bucketTails = NULL;
	m_entryStorage = NULL;
	m_entryCapacity = 0;
	m_bucketCount = 0;
	m_vtable = &g_abstractRendererLookupDispatch;
}

// FUNCTION: LEGORACERS2 0x004bd7c0
void RendererLookupTableState::InsertOrdered(LegoU32 p_key, LegoU32 p_unk0x0c, LegoU32 p_unk0x08, LegoS32 p_hashKey)
{
	if (m_itemCount >= (LegoS32) m_entryCapacity) {
		RendererLookupDispatchView* dispatch = (RendererLookupDispatchView*) m_vtable;
		dispatch->m_flush(this, (void*) m_vtable);
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

// STUB: LEGORACERS2 0x004bd830
void RendererLookupTableState::InsertLinear(LegoU32 p_key, LegoU32 p_unk0x0c, LegoU32 p_unk0x08, LegoS32 p_ignoredHash)
{
	Entry* entry = &m_entryStorage[m_itemCount++];
	entry->m_key = p_key;
	entry->m_next = (Entry*) *m_bucketTails;
	entry->m_unk0x08 = p_unk0x08;
	entry->m_unk0x0c = p_unk0x0c;
	*m_bucketTails = (Entry*) entry;
}
