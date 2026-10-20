#ifndef LR2_RENDERERLOOKUPSTATE_H
#define LR2_RENDERERLOOKUPSTATE_H

#include "decomp.h"
#include "types.h"

struct RendererLookupGlobals {
	LegoU32 m_mode;
	LegoU32 m_unk0x04;
	LegoU8 m_selectionOriginInitialized;
	LegoU8 m_padding[7];
	LegoFloat m_selectionOrigin[3];
};
DECOMP_SIZE_ASSERT(RendererLookupGlobals, 0x1c);
extern RendererLookupGlobals g_unk0x0058a170;
extern LegoFloat g_modelLodDepthThreshold;

LegoBool32 __fastcall IsLookupPointWithinRadius(void*, void*, const LegoFloat*, LegoFloat);
const LegoFloat* __fastcall GetLookupResourceOrigin(LegoU8*, void*);

#endif
