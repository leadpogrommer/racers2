#include "platformmemory.h"

#include <windows.h>

// FUNCTION: LEGORACERS2 0x004e7b90
void* AllocateAlignedMemory(LegoU32 p_bytes, LegoU32 p_alignment)
{
	LegoU32 address = (LegoU32) GlobalAlloc(GMEM_ZEROINIT, p_bytes + p_alignment);
	return (void*) ((address + p_alignment - 1) & ~(p_alignment - 1));
}
