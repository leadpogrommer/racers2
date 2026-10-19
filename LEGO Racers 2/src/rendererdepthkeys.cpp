#include "gameobjectrenderprep.h"
#include "modelrendering.h"

#include <string.h>

// STUB: LEGORACERS2 0x004bdf00
LegoS32 __fastcall GetModelPointSortKey(DisplayRenderer* p_renderer, void*, const Vector3* p_point)
{
	LegoFloat depth = GetGameObjectViewDepth(g_unk0x00679ef0, NULL, p_point);
	return GetModelDepthSortKey(p_renderer, NULL, depth);
}

// STUB: LEGORACERS2 0x004bdf30
LegoS32 __fastcall GetModelDepthSortKey(DisplayRenderer*, void*, LegoFloat p_depth)
{
	LegoU32 bits;
	memcpy(&bits, &p_depth, sizeof(bits));
	if ((bits & 0x80000000) != 0) {
		return 0;
	}
	LegoFloat value =
		8192.0f - (8192.0f - p_depth) * (8192.0f - p_depth) * (8192.0f - p_depth) *
		(8192.0f - p_depth) * 1.8189894035458565e-12f;
	LegoS32 result;
	// Retail uses FISTP under the current FPU rounding mode, rather than FTOL.
	__asm {
		fld value
		fistp result
	}
	return result;
}
