#ifndef LR2_HEAPADDRESSWALK_H
#define LR2_HEAPADDRESSWALK_H

#include "types.h"

class MemoryHeap;

void __fastcall WalkHeapAddressRange(MemoryHeap* p_heap, void*);

#endif
