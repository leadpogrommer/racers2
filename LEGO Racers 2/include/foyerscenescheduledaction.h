#ifndef LR2_FOYERSCENESCHEDULEDACTION_H
#define LR2_FOYERSCENESCHEDULEDACTION_H

#include "decomp.h"
#include "types.h"
#include "vector3.h"

#include <stddef.h>

// Timed action record used by the foyer camera/target animation callbacks.
// The callback tables contain ordinary function pointers, not a C++ vtable.
struct FoyerSceneScheduledAction {
	LegoU8 m_opaque0x00[4];
	void* m_owner; // 0x04
	LegoU32 m_unk0x08;
	LegoFloat m_output;            // 0x0c
	LegoFloat m_startValue;        // 0x10
	LegoFloat m_referenceDistance; // 0x14
	LegoFloat m_progress;          // 0x18
	LegoU32 m_callbackIndex;       // 0x1c
	LegoU8 m_opaque0x20[4];
	LegoU32 m_interpolationIndex; // 0x24
	LegoFloat m_valueAt0x28;
	LegoU32 m_unk0x2c;
	LegoU32 m_unk0x30;
	LegoU32 m_state; // 0x34
	LegoU8 m_opaque0x38[0x18];
	void* m_origin; // 0x50
	LegoU8 m_opaque0x54[0x10];
	void* m_target;                   // 0x64
	void* m_source;                   // 0x68
	void* m_sceneTarget;              // 0x6c
	LegoFloat m_startTime;            // 0x70
	LegoFloat m_endTime;              // 0x74
	LegoFloat m_elapsed;              // 0x78
	LegoFloat m_inverseStartDuration; // 0x7c
	LegoFloat m_phaseScale;           // 0x80
	LegoFloat m_endThreshold;         // 0x84
	LegoFloat m_inverseEndDuration;   // 0x88
	LegoFloat m_callbackScale;        // 0x8c
	LegoU32 m_firstUpdate;            // 0x90
	LegoU8 m_opaque0x94[0x68];
	LegoFloat m_animationScale; // 0xfc

	LegoU32 Dispatch(LegoFloat p_elapsed);
};

DECOMP_SIZE_ASSERT(FoyerSceneScheduledAction, 0x100);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneScheduledAction, m_owner) == 0x04);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneScheduledAction, m_callbackIndex) == 0x1c);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneScheduledAction, m_interpolationIndex) == 0x24);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneScheduledAction, m_valueAt0x28) == 0x28);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneScheduledAction, m_sceneTarget) == 0x6c);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneScheduledAction, m_elapsed) == 0x78);
DECOMP_STATIC_ASSERT(offsetof(FoyerSceneScheduledAction, m_firstUpdate) == 0x90);

#endif // LR2_FOYERSCENESCHEDULEDACTION_H
