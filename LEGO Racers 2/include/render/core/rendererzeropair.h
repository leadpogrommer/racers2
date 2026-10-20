#ifndef LR2_RENDERERZEROPAIR_H
#define LR2_RENDERERZEROPAIR_H

#include "decomp.h"
#include "types.h"

// Two-word record initialized by 0x004abe90. Retail places arrays of these
// records at stride 8 in the renderer owner.
class RendererZeroPair {
public:
	RendererZeroPair();

	LegoU32 m_first;  // 0x00
	LegoU32 m_second; // 0x04
};

DECOMP_SIZE_ASSERT(RendererZeroPair, 0x8);

#endif // LR2_RENDERERZEROPAIR_H
