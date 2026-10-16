#ifndef LR2_MEMORYHEAP_H
#define LR2_MEMORYHEAP_H

#include "heapblock.h"

class MemoryHeap {
public:
	void FUN_004a4040();
	void* AllocateBlock(LegoU32 p_size, LegoBool32 p_forward);
	void FreeBlock(void* p_memory);
	void UpdateCursor();
	HeapBlock* FindForward(HeapBlock** p_start, LegoU32 p_units);
	HeapBlock* FindBackward(HeapBlock** p_start, LegoU32 p_units);
	void InitializeBlock(HeapBlock* p_block, void* p_end, LegoU32 p_size);

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

#endif // LR2_MEMORYHEAP_H
