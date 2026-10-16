#include "memoryheap.h"

// GLOBAL: LEGORACERS2 0x00578c58
HeapBlock* g_freeListHead = NULL;

// GLOBAL: LEGORACERS2 0x00578c5c
HeapBlock* g_freeListTail = NULL;

// GLOBAL: LEGORACERS2 0x00578c98
HeapBlock* g_freeListCursor = NULL;

// FUNCTION: LEGORACERS2 0x004a4040
void MemoryHeap::FUN_004a4040()
{
	m_unk0x0c = TRUE;
}

// STUB: LEGORACERS2 0x004a41d0
void* MemoryHeap::AllocateBlock(LegoU32 p_size, LegoBool32 p_forward)
{
	LegoU32 units = ((p_size + 15) >> 4) + 1;
	HeapBlock* block;
	if (p_forward) {
		block = FindForward(&g_freeListCursor, units);
	}
	else {
		block = FindBackward(&g_freeListTail, units);
	}
	if (block == NULL) {
		return NULL;
	}

	LegoU32 available = block->m_units;
	if (available - units <= 1) {
		if (p_forward) {
			g_freeListCursor = g_freeListCursor->m_next;
			if (g_freeListCursor == NULL) {
				g_freeListCursor = g_freeListHead;
			}
		}
		else {
			UpdateCursor();
		}
		if (block->m_previous != NULL) {
			block->m_previous->m_next = block->m_next;
		}
		else {
			g_freeListHead = block->m_next;
		}
		if (block->m_next != NULL) {
			block->m_next->m_previous = block->m_previous;
		}
		else {
			g_freeListTail = block->m_previous;
		}
		block->m_next = NULL;
		block->m_previous = NULL;
		if (block == g_freeListCursor) {
			UpdateCursor();
		}
		--m_freeBlocks;
		m_freeUnits -= available;
		return block + 1;
	}

	HeapBlock* allocated;
	if (p_forward) {
		allocated = block;
		HeapBlock* remainder = block + units;
		remainder->m_units = block->m_units - units;
		remainder->m_next = block->m_next;
		remainder->m_previous = block->m_previous;
		block->m_units = units;
		block->m_next = NULL;
		block->m_previous = NULL;
		if (remainder->m_previous == NULL) {
			g_freeListHead = remainder;
		}
		else {
			remainder->m_previous->m_next = remainder;
		}
		if (remainder->m_next != NULL) {
			remainder->m_next->m_previous = remainder;
		}
		else {
			g_freeListTail = remainder;
		}
		g_freeListCursor = remainder;
		g_freeListCursor = g_freeListCursor->m_next;
		if (g_freeListCursor == NULL) {
			g_freeListCursor = g_freeListHead;
		}
	}
	else {
		block->m_units = available - units;
		allocated = block + (available - units);
		allocated->m_units = units;
		allocated->m_next = NULL;
		allocated->m_previous = NULL;
	}
	m_freeUnits -= units;
	return allocated + 1;
}

// STUB: LEGORACERS2 0x004a4350
void MemoryHeap::FreeBlock(void* p_memory)
{
	HeapBlock* block = (HeapBlock*) p_memory - 1;
	m_freeUnits += block->m_units;
	HeapBlock* current = g_freeListHead;
	HeapBlock* next = current->m_next;
	HeapBlock** link = &g_freeListHead;
	while (block >= next && next != NULL) {
		link = &current->m_next;
		current = current->m_next;
		next = current->m_next;
	}
	if (block > current) {
		block->m_previous = current;
		block->m_next = next;
		current->m_next = block;
		if (next != NULL) {
			next->m_previous = block;
		}
		else {
			g_freeListTail = block;
		}
	}
	else {
		HeapBlock* previous = current->m_previous;
		block->m_next = current;
		block->m_previous = previous;
		*link = block;
		current->m_previous = block;
	}
	++m_freeBlocks;

	HeapBlock* end = block + block->m_units;
	if (current + current->m_units == block) {
		if (end == next) {
			current->m_next = next->m_next;
			if (next->m_next != NULL) {
				next->m_next->m_previous = current;
			}
			current->m_units += next->m_units + block->m_units;
			m_freeBlocks -= 2;
		}
		else {
			current->m_next = block->m_next;
			if (block->m_next != NULL) {
				block->m_next->m_previous = current;
			}
			current->m_units += block->m_units;
			--m_freeBlocks;
		}
		if (current->m_next == NULL) {
			g_freeListTail = current;
			UpdateCursor();
			return;
		}
	}
	else {
		if (end == next) {
			block->m_next = next->m_next;
			if (next->m_next != NULL) {
				next->m_next->m_previous = block;
			}
			else {
				g_freeListTail = block;
			}
			block->m_units += next->m_units;
			--m_freeBlocks;
		}
		else if (end == current) {
			block->m_next = current->m_next;
			if (current->m_next != NULL) {
				current->m_next->m_previous = block;
			}
			else {
				g_freeListTail = block;
			}
			block->m_units += current->m_units;
			--m_freeBlocks;
		}
	}
	UpdateCursor();
}

// STUB: LEGORACERS2 0x004a4480
void MemoryHeap::UpdateCursor()
{
	HeapBlock* block = g_freeListHead;
	HeapBlock* previous;
	HeapBlock* next;
	do {
		next = block->m_next;
		if (next != NULL) {
			if (g_freeListCursor >= block && g_freeListCursor < next) {
				g_freeListCursor = block;
				return;
			}
		}
		else if (block >= g_freeListCursor) {
			g_freeListCursor = block;
			return;
		}
		previous = block;
		block = next;
	} while (block != NULL);
	g_freeListCursor = previous;
}

// STUB: LEGORACERS2 0x004a44c0
HeapBlock* MemoryHeap::FindForward(HeapBlock** p_start, LegoU32 p_units)
{
	HeapBlock* block = *p_start;
	do {
		if (block->m_units >= p_units) {
			return block;
		}
		block = block->m_next;
		if (block == NULL) {
			block = g_freeListHead;
		}
	} while (block != *p_start);
	return NULL;
}

// FUNCTION: LEGORACERS2 0x004a44f0
HeapBlock* MemoryHeap::FindBackward(HeapBlock** p_start, LegoU32 p_units)
{
	HeapBlock* block = *p_start;
	do {
	} while (block->m_units < p_units && (block = block->m_previous) != NULL);
	return block;
}

// FUNCTION: LEGORACERS2 0x004a4510
void MemoryHeap::InitializeBlock(HeapBlock* p_block, void* p_end, LegoU32 p_size)
{
	p_block->m_units = p_size >> 4;
	p_block->m_next = NULL;
	p_block->m_previous = NULL;
}
