#include "memoryheap.h"

#include "application.h"
#include "diagnostics.h"
#include "platformmemory.h"

#include <new.h>
#include <stdlib.h>

// GLOBAL: LEGORACERS2 0x00578c58
HeapBlock* g_heapFreeListHead = NULL;

// GLOBAL: LEGORACERS2 0x00578c5c
HeapBlock* g_heapFreeListTail = NULL;

// GLOBAL: LEGORACERS2 0x00578c98
HeapBlock* g_heapFreeListCursor = NULL;

// GLOBAL: LEGORACERS2 0x00578c64
void* g_heapArenaStart = NULL;

// GLOBAL: LEGORACERS2 0x00578c68
LegoU32 g_heapArenaCapacity = 0;

// GLOBAL: LEGORACERS2 0x00578c9c
LegoBool32 g_heapInitializationSucceeded = FALSE;

// GLOBAL: LEGORACERS2 0x00578ca0
static MemoryHeap* g_memoryHeapSingleton = NULL;

// GLOBAL: LEGORACERS2 0x00578c60
static LegoU8 g_memoryHeapInitialized = 0;

// GLOBAL: LEGORACERS2 0x00578c70
static LegoU32 g_memoryHeapStorage[sizeof(MemoryHeap) / sizeof(LegoU32)];

static void DestroyMemoryHeapAtExit();

// STUB: LEGORACERS2 0x004a3ed0
MemoryHeap* GetMemoryHeap()
{
	if (g_memoryHeapSingleton == NULL) {
		if ((g_memoryHeapInitialized & 1) == 0) {
			g_memoryHeapInitialized |= 1;
			new (g_memoryHeapStorage) MemoryHeap;
			atexit(DestroyMemoryHeapAtExit);
		}
		g_memoryHeapSingleton = (MemoryHeap*) g_memoryHeapStorage;
	}
	return g_memoryHeapSingleton;
}

// FUNCTION: LEGORACERS2 0x004a3f20 FOLDED
static void DestroyMemoryHeapAtExit()
{
}

// STUB: LEGORACERS2 0x004a3f30
void MemoryHeap::EnterOperation()
{
	if (m_entered == TRUE) {
		ReportFatalError("Thread overlap, make this wait till its ok");
	}
	m_entered = TRUE;
}

// FUNCTION: LEGORACERS2 0x004a3f60
void MemoryHeap::LeaveOperation()
{
	if (m_entered == FALSE) {
		ReportFatalError("Exiting function without entering???");
	}
	m_entered = FALSE;
}

// STUB: LEGORACERS2 0x004a3f80
MemoryHeap::MemoryHeap()
{
	m_unk0x0c = 0;
	m_entered = FALSE;
	m_useCRT = FALSE;
	g_heapFreeListCursor = NULL;

	LegoU32 capacity = GetApplication()->GetHeapReserveSize();
	while (TRUE) {
		m_start = AllocateAlignedMemory(capacity, 16);
		if (m_start != NULL) {
			break;
		}
		capacity -= 0x2000;
	}

	g_heapArenaStart = m_start;
	g_heapArenaCapacity = capacity;
	m_unk0x08 = m_start;
	m_capacity = capacity;
	m_freeBlocks = 1;
	m_end = (char*) m_start + capacity;

	InitializeBlock((HeapBlock*) m_start, m_end, capacity);
	g_heapFreeListHead = (HeapBlock*) m_start;
	g_heapFreeListTail = (HeapBlock*) m_start;
	g_heapFreeListCursor = (HeapBlock*) m_start;
	g_heapFreeListHead->m_next = NULL;
	g_heapFreeListHead->m_previous = NULL;
	m_freeUnits = g_heapFreeListHead->m_units;

	// Retail probes a near-full forward allocation, frees it, and records that
	// the list can service both paths. Startup observes the flag at 0x578c9c.
	void* probe = AllocateBlock(capacity - 0x810, TRUE);
	if (probe != NULL) {
		FreeBlock(probe);
		g_heapInitializationSucceeded = TRUE;
	}
}

// FUNCTION: LEGORACERS2 0x004a4040
void MemoryHeap::SetUnknownFlag()
{
	m_unk0x0c = TRUE;
}

// STUB: LEGORACERS2 0x004a4050
void* MemoryHeap::Allocate(LegoU32 p_bytes, const LegoChar* p_diagnosticTag)
{
	if (m_useCRT) {
		return malloc(p_bytes);
	}

	EnterOperation();
	LegoU32 alignedBytes = (p_bytes + 15) & ~15;
	void* memory = AllocateBlock(alignedBytes, TRUE);
	LegoBool32 developmentMode = GetApplication()->IsDevelopmentMode();
	if (memory == NULL) {
		if (developmentMode) {
			ReportFatalError("Out of memory");
		}
		PrintDiagnostic("Out of memory mallocing:%d bytes", alignedBytes);
		(void) p_diagnosticTag;
	}
	LeaveOperation();
	return memory;
}

// STUB: LEGORACERS2 0x004a40d0
void* MemoryHeap::AllocateFromEnd(LegoU32 p_bytes, const LegoChar* p_diagnosticTag)
{
	if (m_useCRT) {
		return malloc(p_bytes);
	}

	EnterOperation();
	LegoU32 alignedBytes = (p_bytes + 15) & ~15;
	void* memory = AllocateBlock(alignedBytes, FALSE);
	LegoBool32 developmentMode = GetApplication()->IsDevelopmentMode();
	if (memory == NULL) {
		if (developmentMode) {
			ReportFatalError("Out of memory");
		}
		PrintDiagnostic("Out of memory mallocing:%d bytes", alignedBytes);
		(void) p_diagnosticTag;
	}
	LeaveOperation();
	return memory;
}

// FUNCTION: LEGORACERS2 0x004a4150
void MemoryHeap::Free(void* p_memory)
{
	if (m_useCRT) {
		free(p_memory);
		return;
	}

	EnterOperation();
	FreeBlock(p_memory);
	LeaveOperation();
}

// FUNCTION: LEGORACERS2 0x004a4190
void* AllocateHeapMemory(LegoU32 p_bytes)
{
	return GetMemoryHeap()->Allocate(p_bytes, "Memory for New");
}

// FUNCTION: LEGORACERS2 0x004a41b0
void FreeHeapMemory(void* p_memory)
{
	GetMemoryHeap()->Free(p_memory);
}

// STUB: LEGORACERS2 0x004a41d0
void* MemoryHeap::AllocateBlock(LegoU32 p_bytes, LegoBool32 p_forward)
{
	LegoU32 units = ((p_bytes + 15) >> 4) + 1;
	HeapBlock* block = p_forward ? FindForward(&g_heapFreeListCursor, units) : FindBackward(&g_heapFreeListTail, units);
	if (block == NULL) {
		return NULL;
	}

	LegoU32 available = block->m_units;
	if (available - units <= 1) {
		if (p_forward) {
			g_heapFreeListCursor = g_heapFreeListCursor->m_next;
			if (g_heapFreeListCursor == NULL) {
				g_heapFreeListCursor = g_heapFreeListHead;
			}
		}
		else {
			UpdateCursor();
		}

		if (block->m_previous != NULL) {
			block->m_previous->m_next = block->m_next;
		}
		else {
			g_heapFreeListHead = block->m_next;
		}
		if (block->m_next != NULL) {
			block->m_next->m_previous = block->m_previous;
		}
		else {
			g_heapFreeListTail = block->m_previous;
		}
		block->m_next = NULL;
		block->m_previous = NULL;
		if (block == g_heapFreeListCursor) {
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
			g_heapFreeListHead = remainder;
		}
		else {
			remainder->m_previous->m_next = remainder;
		}
		if (remainder->m_next != NULL) {
			remainder->m_next->m_previous = remainder;
		}
		else {
			g_heapFreeListTail = remainder;
		}
		g_heapFreeListCursor = remainder->m_next;
		if (g_heapFreeListCursor == NULL) {
			g_heapFreeListCursor = g_heapFreeListHead;
		}
	}
	else {
		block->m_units = available - units;
		allocated = block + block->m_units;
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
	HeapBlock* current = g_heapFreeListHead;
	HeapBlock* next = current->m_next;
	HeapBlock** link = &g_heapFreeListHead;
	while (next != NULL && (LegoU32) block >= (LegoU32) next) {
		link = &current->m_next;
		current = next;
		next = current->m_next;
	}
	if ((LegoU32) block > (LegoU32) current) {
		block->m_previous = current;
		block->m_next = next;
		current->m_next = block;
		if (next != NULL) {
			next->m_previous = block;
		}
		else {
			g_heapFreeListTail = block;
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
			g_heapFreeListTail = current;
			UpdateCursor();
			return;
		}
	}
	else if (end == next) {
		block->m_next = next->m_next;
		if (next->m_next != NULL) {
			next->m_next->m_previous = block;
		}
		else {
			g_heapFreeListTail = block;
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
			g_heapFreeListTail = block;
		}
		block->m_units += current->m_units;
		--m_freeBlocks;
	}
	UpdateCursor();
}

// FUNCTION: LEGORACERS2 0x004a4480
void MemoryHeap::UpdateCursor()
{
	HeapBlock* block = g_heapFreeListHead;
	HeapBlock* previous;
	do {
		HeapBlock* next = block->m_next;
		if (next != NULL) {
			if ((LegoU32) g_heapFreeListCursor >= (LegoU32) block && (LegoU32) g_heapFreeListCursor < (LegoU32) next) {
				g_heapFreeListCursor = block;
				return;
			}
		}
		else if ((LegoU32) block >= (LegoU32) g_heapFreeListCursor) {
			g_heapFreeListCursor = block;
			return;
		}
		previous = block;
		block = next;
	} while (block != NULL);
	g_heapFreeListCursor = previous;
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
			block = g_heapFreeListHead;
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
	(void) p_end;
}
