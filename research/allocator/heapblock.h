#ifndef LR2_HEAPBLOCK_H
#define LR2_HEAPBLOCK_H

#include "decomp.h"
#include "types.h"

struct HeapBlock {
	HeapBlock* m_next;     // 0x00
	HeapBlock* m_previous; // 0x04
	LegoU32 m_units;       // 0x08: includes the header; units are 16 bytes
	undefined4 m_unk0x0c;  // 0x0c
};

// Allocation returns the address 16 bytes after the block header.
DECOMP_SIZE_ASSERT(HeapBlock, 0x10);

#endif // LR2_HEAPBLOCK_H
