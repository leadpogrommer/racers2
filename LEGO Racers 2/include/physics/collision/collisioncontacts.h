#ifndef LR2_COLLISIONCONTACTS_H
#define LR2_COLLISIONCONTACTS_H

#include "game/objects/gamemanager.h"
#include "math/vector3.h"

struct CollisionContact {
	void SwapParticipants(const Vector3* p_offset, GameLifecycleComponent* p_first, GameLifecycleComponent* p_second);
	Vector3 m_point;                  // 0x00
	Vector3 m_normal;                 // 0x0c
	Vector3 m_separationAxis;         // 0x18
	LegoFloat m_penetration;          // 0x24
	GameLifecycleComponent* m_first;  // 0x28
	GameLifecycleComponent* m_second; // 0x2c
	Vector3 m_relativeVelocity;       // 0x30
	Vector3 m_secondOffset;           // 0x3c
	LegoU16 m_flags;                  // 0x48
	LegoU16 m_unk0x4a;
};
DECOMP_SIZE_ASSERT(CollisionContact, 0x4c);

#endif // LR2_COLLISIONCONTACTS_H
