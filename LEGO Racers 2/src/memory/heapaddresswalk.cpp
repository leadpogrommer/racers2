#include "memory/heapaddresswalk.h"

#include "memory/memoryheap.h"

// STUB: LEGORACERS2 0x004a4530
void __fastcall WalkHeapAddressRange(MemoryHeap* p_heap, void*)
{
	// Retail performs arithmetic on local addresses only. It does not touch the
	// represented memory or change heap state. Keep that distinction explicit.
	LegoU32 first = (LegoU32) p_heap->m_start + 1;
	LegoU32 capacity = p_heap->m_capacity;
	for (LegoU32 pass = 0; pass < 2; ++pass) {
		LegoU32 cursor = first;
		LegoU32 remaining;
		for (remaining = 1; remaining < capacity; ++remaining) {
			++cursor;
		}
		cursor = (LegoU32) p_heap->m_end - 1;
		for (remaining = 1; remaining < capacity; ++remaining) {
			--cursor;
		}
	}
}
