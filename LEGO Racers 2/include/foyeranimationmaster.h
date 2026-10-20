#ifndef LR2_FOYERANIMATIONMASTER_H
#define LR2_FOYERANIMATIONMASTER_H

#include "charbuffer24.h"
#include "decomp.h"
#include "quaternion.h"
#include "vector3.h"

#include <stddef.h>

struct FoyerAnimationMasterEntry {
	LegoChar m_directory[0x100];
	CharBuffer24 m_descriptor;   // 0x100
	LegoU32 m_used;              // 0x118
	LegoS32 m_references;        // 0x11c
	LegoU8* m_body;              // 0x120
	LegoS32 m_flags;             // 0x124
	LegoU32 m_quaternionCount;   // 0x128
	LegoS32 m_frameCount;        // 0x12c
	LegoFloat m_frameCountFloat; // 0x130
	LegoU32 m_frameStride;       // 0x134
	LegoFloat m_frameRate;       // 0x138
	LegoFloat m_frameSeconds;    // 0x13c
	LegoFloat m_unk0x140;
	LegoFloat m_duration; // 0x144
};
DECOMP_SIZE_ASSERT(FoyerAnimationMasterEntry, 0x148);
DECOMP_STATIC_ASSERT(offsetof(FoyerAnimationMasterEntry, m_body) == 0x120);
DECOMP_STATIC_ASSERT(offsetof(FoyerAnimationMasterEntry, m_frameRate) == 0x138);

struct FoyerAnimationMasterCache {
	FoyerAnimationMasterCache();
	~FoyerAnimationMasterCache();
	LegoS32 FindOrAdd(const LegoChar*, const CharBuffer24*, LegoU32 p_retain);
	void Retain(LegoU32);
	void Release(LegoU32);
	LegoU32 m_usedCount;
	FoyerAnimationMasterEntry m_entries[128];
};
DECOMP_SIZE_ASSERT(FoyerAnimationMasterCache, 0xa404);
FoyerAnimationMasterCache* AcquireFoyerAnimationMasterCache();
void ReleaseFoyerAnimationMasterCache();

struct FoyerAnimationPose {
	LegoFloat m_phase;
	Vector3 m_position;
	Vector3 m_velocity;
	Quaternion* m_quaternions;
};
DECOMP_SIZE_ASSERT(FoyerAnimationPose, 0x20);
DECOMP_STATIC_ASSERT(offsetof(FoyerAnimationPose, m_position) == 4);
DECOMP_STATIC_ASSERT(offsetof(FoyerAnimationPose, m_quaternions) == 0x1c);

struct FoyerAnimationPlayer {
	FoyerAnimationPlayer(LegoU32 p_skeletonCount);
	~FoyerAnimationPlayer();
	void Bind(const FoyerAnimationMasterEntry*, LegoFloat p_rate, LegoFloat p_weight);
	void Update(LegoFloat p_step, LegoU32 p_updateQuaternions);
	LegoU32 m_state;
	const FoyerAnimationMasterEntry* m_track;
	LegoU32 m_skeletonCount;
	LegoFloat m_rate;
	LegoFloat m_weight;
	FoyerAnimationPose m_pose; // 0x14
};
DECOMP_SIZE_ASSERT(FoyerAnimationPlayer, 0x34);
DECOMP_STATIC_ASSERT(offsetof(FoyerAnimationPlayer, m_pose) == 0x14);

#endif
