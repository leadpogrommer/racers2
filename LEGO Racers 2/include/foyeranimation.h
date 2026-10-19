#ifndef LR2_FOYERANIMATION_H
#define LR2_FOYERANIMATION_H

#include "charbuffer24.h"
#include "decomp.h"
#include "types.h"

#include <stddef.h>

// These projections cover only fields observed by the animation helpers used
// by foyer scene events. They do not claim the complete controller or track
// record layouts.
struct FoyerAnimationLayerView {
	LegoU32 m_unk0x00;
	const void* m_track;
	LegoU32 m_unk0x08;
	LegoFloat m_unk0x0c;
	LegoFloat m_unk0x10;
	LegoU32 m_unk0x14;
};

struct FoyerAnimationControllerView {
	LegoU8 m_opaque0x00[0x84];
	LegoU32 m_enabled;
	LegoU8 m_opaque0x88[8];
	FoyerAnimationLayerView* m_layers[4];
	void* m_trackRecords;
	LegoS32 m_trackCount;
	LegoS32 m_trackIds[1];
};

DECOMP_STATIC_ASSERT(offsetof(FoyerAnimationControllerView, m_layers) == 0x90);
DECOMP_STATIC_ASSERT(offsetof(FoyerAnimationControllerView, m_enabled) == 0x84);
DECOMP_STATIC_ASSERT(offsetof(FoyerAnimationLayerView, m_track) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(FoyerAnimationLayerView, m_unk0x14) == 0x14);
DECOMP_STATIC_ASSERT(offsetof(FoyerAnimationControllerView, m_trackRecords) == 0xa0);
DECOMP_STATIC_ASSERT(offsetof(FoyerAnimationControllerView, m_trackCount) == 0xa4);
DECOMP_STATIC_ASSERT(offsetof(FoyerAnimationControllerView, m_trackIds) == 0xa8);

LegoS32 __fastcall FindFoyerAnimationTrack(FoyerAnimationControllerView*, void*, const LegoChar*, LegoU32);
void __fastcall StartFoyerAnimation(FoyerAnimationControllerView*, void*, const LegoChar*, LegoU32);
void __fastcall ApplyFoyerAnimationTrack(FoyerAnimationControllerView*, void*, LegoU32, LegoU32);
void __fastcall ResetFoyerAnimationLayer(FoyerAnimationControllerView*, void*, LegoU32);
void __fastcall SetFoyerAnimationLayerValues(FoyerAnimationControllerView*, void*, LegoU32, LegoU32, LegoU32, LegoU32);
void __fastcall SetFoyerAnimationLayerValue(FoyerAnimationControllerView*, void*, LegoU32, LegoU32);
void __fastcall SetFoyerAnimationLayerValueReordered(FoyerAnimationControllerView*, void*, LegoU32, LegoU32);
LegoFloat __fastcall GetFoyerAnimationLayerValue(FoyerAnimationControllerView*, void*, LegoU32);
void __fastcall SetFoyerAnimationBlend(FoyerAnimationControllerView*, void*, LegoFloat);
void __cdecl InitializeFoyerAnimationTiming();

extern LegoFloat g_foyerAnimationTickScale;
extern LegoFloat g_foyerAnimationBlend;

#endif // LR2_FOYERANIMATION_H
