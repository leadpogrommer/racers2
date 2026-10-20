#ifndef LR2_FOYERSCENEEVENTS_H
#define LR2_FOYERSCENEEVENTS_H

#include "core/strings/charbuffer24.h"
#include "decomp.h"
#include "types.h"

#include <stddef.h>

struct FoyerSceneEventPrefix {
	LegoU32 m_unk0x00;
	LegoU32 m_unk0x04;
	LegoU32 m_unk0x08;
	CharBuffer24 m_firstBuffer;
	LegoU32 m_unk0x24;
	CharBuffer24 m_secondBuffer;
	LegoU32 m_unk0x40;
	LegoU32 m_unk0x44;
	LegoU32 m_hasPendingAnimation;
	LegoFloat m_animationElapsed;
	LegoFloat m_animationDuration;
	LegoU32 m_animationLayer;
	LegoFloat m_layerWeight0;
	LegoFloat m_layerWeight1;
	LegoU8 m_opaque0x60[8];
	LegoU32 m_faceState;
	LegoFloat m_faceElapsed;
	LegoFloat m_faceDuration;
	LegoU32 m_faceToggle;
	LegoFloat m_nextFaceTime;
	LegoU32 m_unk0x7c;
	LegoU32 m_hasAttachedTarget;
	void* m_attachedTarget;
	LegoFloat m_attachedOffset[3];
	LegoU32 m_unk0x90;
};

struct FoyerSceneResourceView;

DECOMP_STATIC_ASSERT(offsetof(FoyerSceneEventPrefix, m_firstBuffer) == 0x0c);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneEventPrefix, m_unk0x24) == 0x24);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneEventPrefix, m_secondBuffer) == 0x28);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneEventPrefix, m_animationElapsed) == 0x4c);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneEventPrefix, m_animationLayer) == 0x54);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneEventPrefix, m_faceState) == 0x68);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneEventPrefix, m_attachedTarget) == 0x84);

FoyerSceneEventPrefix* __fastcall InitializeFoyerSceneEvent(FoyerSceneEventPrefix*, void*);
void __fastcall SetFoyerSceneEventTarget(FoyerSceneEventPrefix*, void*, void* p_target);
void __fastcall SetFoyerSceneEventReady(FoyerSceneEventPrefix*, void*);
void __fastcall SetFoyerSceneEventAnimation(FoyerSceneEventPrefix*, void*, const LegoChar*, LegoFloat, LegoFloat);
void __fastcall UpdateFoyerSceneEntry(FoyerSceneEventPrefix*, void*, LegoFloat p_elapsedSeconds);
void __fastcall UpdateFoyerSceneEvents(FoyerSceneResourceView*, void*, LegoFloat p_elapsedSeconds);

#endif // LR2_FOYERSCENEEVENTS_H
