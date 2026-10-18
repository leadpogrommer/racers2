#ifndef LR2_CONTACTCACHE_H
#define LR2_CONTACTCACHE_H

#include "cachedconstraint.h"
#include "gamemanager.h"

struct CollisionContact;
class ContactCacheController : public GameObject {
public:
	ContactCacheController();
	~ContactCacheController();
	void ResetContacts();
	void CollectContacts(CollisionContact* p_contacts, LegoS32 p_count);
	LegoU32 PrepareContacts();
	LegoU32 UpdateContacts();
	CachedCollisionContact m_contacts[128]; // 0x58
	LegoS32 m_count;                        // 0x6658
};
DECOMP_STATIC_ASSERT(offsetof(ContactCacheController, m_count) == 0x6658);
DECOMP_SIZE_ASSERT(ContactCacheController, 0x665c);
extern ContactCacheController g_contactCacheController;
#endif
