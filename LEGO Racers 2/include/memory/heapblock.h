#ifndef LR2_HEAPBLOCK_H
#define LR2_HEAPBLOCK_H

#include "decomp.h"
#include "types.h"

// The arena stores free-list links and the capacity of each 16-byte unit run.
// The unknown fourth word is preserved when blocks are split or coalesced.
struct HeapBlock {
	HeapBlock* m_next;     // 0x00
	HeapBlock* m_previous; // 0x04
	LegoU32 m_units;       // 0x08, including this header
	undefined4 m_unk0x0c;  // 0x0c
};

DECOMP_SIZE_ASSERT(HeapBlock, 0x10);

#endif // LR2_HEAPBLOCK_H
