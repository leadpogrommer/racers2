#include "contactcache.h"

#include "collisioncontacts.h"
#include "gameobjectdispatch.h"
#include "memoryheap.h"

static GameObject* __fastcall DeleteContactCacheController(GameObject*, void*, LegoU32);
static LegoU32 __fastcall ContactCacheAllocationSize(GameObject*, void*);
static LegoU32 __fastcall UpdateCachedContacts(GameObject* p_object, void*)
{
	return ((ContactCacheController*) p_object)->UpdateContacts();
}
static LegoU32 __fastcall PrepareCachedContacts(GameObject* p_object, void*)
{
	return ((ContactCacheController*) p_object)->PrepareContacts();
}

struct ContactCacheDispatch {
	GameObjectDispatchView m_base;
	LegoU32(__fastcall* m_size)(GameObject*, void*);
};
static const ContactCacheDispatch g_contactCacheDispatch = {
	{DeleteContactCacheController,
	 GameObjectBaseApplyChildTransform,
	 GameObjectBaseNoOp,
	 GameObjectBaseNoOp,
	 GameObjectBaseInitialize,
	 GameObjectBaseTrue,
	 GameObjectBaseZeroOneArgument,
	 GameObjectBaseRemove,
	 UpdateCachedContacts,
	 PrepareCachedContacts,
	 GameObjectBaseNoOp,
	 GameObjectBaseNoOp,
	 GameObjectBaseDrawChild,
	 GameObjectBaseNoOpOneArgument,
	 GameObjectBaseZero,
	 GameObjectBaseNoOpTwoArguments,
	 GameObjectBaseZero,
	 GameObjectBaseZeroTwoArguments,
	 GameObjectBaseUpdateRegion,
	 GameObjectBaseNoOpOneArgument},
	ContactCacheAllocationSize
}; // 0x00536c50, twenty-one entries

// GLOBAL: LEGORACERS2 0x0067dd40
ContactCacheController g_contactCacheController;

ContactCacheController::~ContactCacheController()
{
	// The array's destructors restore each constraint's base dispatch in reverse.
}

// STUB: LEGORACERS2 0x004e66c0
ContactCacheController::ContactCacheController() : GameObject(0xe00, NULL, NULL)
{
	m_count = 0;
	m_vtable = &g_contactCacheDispatch.m_base;
}

// STUB: LEGORACERS2 0x004e6730
static LegoU32 __fastcall ContactCacheAllocationSize(GameObject*, void*)
{
	return 0x665c;
}

// STUB: LEGORACERS2 0x004e6740
static GameObject* __fastcall DeleteContactCacheController(GameObject* p_object, void*, LegoU32 p_flags)
{
	((ContactCacheController*) p_object)->~ContactCacheController();
	if (p_flags & 1) {
		FreeHeapMemory(p_object);
	}
	return p_object;
}

// STUB: LEGORACERS2 0x004fbe50
void ContactCacheController::ResetContacts()
{
	m_count = 0;
}

// STUB: LEGORACERS2 0x004fbe60
void ContactCacheController::CollectContacts(CollisionContact* p_contacts, LegoS32 p_count)
{
	typedef void(__fastcall * ContactCallback)(GameLifecycleComponent*, void*, CollisionContact*);
	struct Dispatch {
		undefined m_unknown[0x38];
		ContactCallback m_contact;
	};
	for (LegoS32 i = 0; i < p_count; ++i) {
		CollisionContact& contact = p_contacts[i];
		if ((contact.m_flags & 0x40) == 0) {
			if (contact.m_flags & 0x20) {
				GameLifecycleComponent* second = contact.m_second;
				((const Dispatch*) contact.m_first->m_vtable)->m_contact(contact.m_first, NULL, &contact);
				((const Dispatch*) second->m_vtable)->m_contact(second, NULL, &contact);
			}
			else if (contact.m_flags & 2) {
				if (m_count > 127) {
					return;
				}
				CachedCollisionContact& cached = m_contacts[m_count++];
				cached.CopyContact(&contact);
				cached.m_normal = contact.m_normal;
				cached.m_force.m_x = 0;
				cached.m_force.m_y = 0;
				cached.m_force.m_z = 0;
			}
		}
	}
}

// STUB: LEGORACERS2 0x004fbf50
LegoU32 ContactCacheController::PrepareContacts()
{
	for (LegoS32 i = 0; i < m_count; ++i) {
		const ContactConstraintDispatch* dispatch = (const ContactConstraintDispatch*) m_contacts[i].m_vtable;
		dispatch->m_prepare(&m_contacts[i], NULL);
	}
	return 0;
}

// STUB: LEGORACERS2 0x004fbf90
LegoU32 ContactCacheController::UpdateContacts()
{
	for (LegoS32 i = 0; i < m_count; ++i) {
		const ContactConstraintDispatch* dispatch = (const ContactConstraintDispatch*) m_contacts[i].m_vtable;
		dispatch->m_update(&m_contacts[i], NULL);
	}
	return 0;
}
