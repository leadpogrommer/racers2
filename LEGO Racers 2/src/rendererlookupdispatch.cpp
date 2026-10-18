#include "rendererconstructiondispatch.h"

void __fastcall InsertRendererBucketedLookup(
	RendererLookupTableState* p_table,
	void*,
	LegoU32 p_key,
	LegoU32 p_resource,
	LegoU32 p_record,
	LegoS32 p_hash
)
{
	p_table->InsertOrdered(p_key, p_resource, p_record, p_hash);
}

void __fastcall InsertRendererOrderedLookup(
	RendererLookupTableState* p_table,
	void*,
	LegoU32 p_key,
	LegoU32 p_resource,
	LegoU32 p_record,
	LegoS32 p_hash
)
{
	p_table->Insert(p_key, p_resource, p_record, p_hash);
}

void __fastcall InsertRendererDeferredLookup(
	RendererLookupTableState* p_table,
	void*,
	LegoU32 p_key,
	LegoU32 p_resource,
	LegoU32 p_record,
	LegoS32 p_hash
)
{
	p_table->InsertLinear(p_key, p_resource, p_record, p_hash);
}

void __fastcall FlushRendererBucketedLookup(RendererLookupTableState* p_table, void*)
{
	p_table->FlushBucketedEntries();
}

void __fastcall FlushRendererOrderedLookup(RendererLookupTableState* p_table, void*)
{
	p_table->FlushOrderedEntries();
}

void __fastcall FlushRendererDeferredLookup(RendererLookupTableState* p_table, void*)
{
	p_table->FlushDeferredDrawResources();
}

// Explicit ABI bridge pointers are verified with check_renderer_main_dispatch.py.
// Original table: 0x005358d4
const RendererLookupDispatch g_rendererBucketedLookupDispatch = {
	InsertRendererBucketedLookup,
	FlushRendererBucketedLookup
};

// Original table: 0x005358cc
const RendererLookupDispatch g_rendererOrderedLookupDispatch = {
	InsertRendererOrderedLookup,
	FlushRendererOrderedLookup
};

// Original table: 0x005358c4
const RendererLookupDispatch g_rendererDeferredLookupDispatch = {
	InsertRendererDeferredLookup,
	FlushRendererDeferredLookup
};
