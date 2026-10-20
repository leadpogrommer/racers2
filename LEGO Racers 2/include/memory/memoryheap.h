#ifndef LR2_MEMORYHEAP_H
#define LR2_MEMORYHEAP_H

#include "memory/heapblock.h"

// The linker folds the empty process-exit callbacks to one bare RET.
// FUNCTION: LEGORACERS2 0x004a3f20 FOLDED
// OnSavedDisplayConfigExit

// FUNCTION: LEGORACERS2 0x004a3f20 FOLDED
// LookupNoOpCleanup

class MemoryHeap {
public:
	MemoryHeap();

	void* Allocate(LegoU32 p_bytes, const LegoChar* p_diagnosticTag);
	void* AllocateFromEnd(LegoU32 p_bytes, const LegoChar* p_diagnosticTag);
	void Free(void* p_memory);
	void* AllocateBlock(LegoU32 p_bytes, LegoBool32 p_forward);
	void FreeBlock(void* p_memory);
	void UpdateCursor();
	HeapBlock* FindForward(HeapBlock** p_start, LegoU32 p_units);
	HeapBlock* FindBackward(HeapBlock** p_start, LegoU32 p_units);
	void InitializeBlock(HeapBlock* p_block, void* p_end, LegoU32 p_size);
	void SetUnknownFlag();
	void EnterOperation();
	void LeaveOperation();

	void* m_start;        // 0x00
	void* m_end;          // 0x04
	void* m_unk0x08;      // 0x08
	LegoU32 m_unk0x0c;    // 0x0c
	LegoU32 m_capacity;   // 0x10
	LegoU32 m_freeUnits;  // 0x14
	LegoU32 m_freeBlocks; // 0x18
	LegoBool32 m_entered; // 0x1c
	LegoBool32 m_useCRT;  // 0x20
};

DECOMP_SIZE_ASSERT(MemoryHeap, 0x24);

// Accessor for the process-wide arena and its simple allocation facade.
MemoryHeap* GetMemoryHeap();
void* AllocateHeapMemory(LegoU32 p_bytes);
void FreeHeapMemory(void* p_memory);

#endif // LR2_MEMORYHEAP_H
