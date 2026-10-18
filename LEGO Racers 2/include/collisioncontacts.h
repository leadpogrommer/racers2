#ifndef LR2_COLLISIONCONTACTS_H
#define LR2_COLLISIONCONTACTS_H

#include "gamemanager.h"
#include "vector3.h"

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

struct CachedCollisionContact {
	undefined m_data[0xcc];
};

class ContactCacheController : public GameObject {
public:
	void ResetContacts();
	void CollectContacts(CollisionContact* p_contacts, LegoS32 p_count);
	CachedCollisionContact m_contacts[128]; // 0x58
	LegoS32 m_count;                        // 0x6658
};
DECOMP_STATIC_ASSERT(offsetof(ContactCacheController, m_count) == 0x6658);
extern ContactCacheController g_contactCacheController;

#endif // LR2_COLLISIONCONTACTS_H
