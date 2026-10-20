#ifndef LR2_GAMEANIMATEDOBJECT_H
#define LR2_GAMEANIMATEDOBJECT_H

#include "game/objects/gamemanager.h"
#include "math/vector3.h"

struct FoyerAnimationControllerView;

// A prefix used by character and scene animation callers. The full allocation
// and other derived-object interfaces remain under investigation.
struct GameAnimatedObject : GameObject {
	GameLifecycleComponent m_transform; // +58
	undefined m_gap0x68[0x18c - 0x68];
	LegoU32 m_behaviorFlags;
	undefined m_gap0x190[8];
	LegoS32 m_animationTransformIndex;
	undefined m_gap0x19c[0x1b4 - 0x19c];
	LegoU32 m_animationTransformEnabled;
	undefined m_gap0x1b8[4];
	LegoFloat m_animationTransformValues[5];
	undefined m_gap0x1d0[0x1e4 - 0x1d0];
	FoyerAnimationControllerView* m_animation;
	undefined m_gap0x1e8[0x2b8 - 0x1e8];
	Vector3 m_facing;

	void UpdateFacing(const LegoFloat* p_orientation);
	void SetFrozen(LegoBool32 p_frozen);
	void SetAnimationTransformEnabled(LegoU32 p_enabled);
};
DECOMP_STATIC_ASSERT(offsetof(GameAnimatedObject, m_transform) == 0x58);
DECOMP_STATIC_ASSERT(offsetof(GameAnimatedObject, m_behaviorFlags) == 0x18c);
DECOMP_STATIC_ASSERT(offsetof(GameAnimatedObject, m_animationTransformIndex) == 0x198);
DECOMP_STATIC_ASSERT(offsetof(GameAnimatedObject, m_animationTransformValues) == 0x1bc);
DECOMP_STATIC_ASSERT(offsetof(GameAnimatedObject, m_animation) == 0x1e4);
DECOMP_STATIC_ASSERT(offsetof(GameAnimatedObject, m_facing) == 0x2b8);

#endif
