#include "game/objects/gamemanager.h"
#include "game/runtime/gameframe.h"
#include "physics/collision/collisioncontacts.h"
#include "physics/dynamics/contactcache.h"

// GLOBAL: LEGORACERS2 0x00549bb8
LegoU32 g_gameMovementSequence = 0x40000000;

// GLOBAL: LEGORACERS2 0x005790d4
LegoFloat g_currentSimulationStep;

namespace
{

LegoBool32 CanMove(const GameObject* p_object)
{
	LegoU32 flags = p_object->m_flags;
	return (flags & 2) == 0 && ((flags >> 3) & 2) == 0;
}

} // namespace

// STUB: LEGORACERS2 0x004e67c0
void GameManager::MoveObjects(LegoFloat p_stepSeconds)
{
	if (m_unk0x04 != 0) {
		return;
	}
	GetInitializedSimulationScheduler();
	// Retail's tracing call 0x00489970(frame, 10) is a bare RET 8.
	++g_gameMovementSequence;
	g_currentSimulationStep = p_stepSeconds;
	RefreshCollisionStates();

	// Retail reuses its stack parameter as the contact-array output slot.
	// Retain the original bits if the no-manager branch leaves it untouched.
	union {
		LegoFloat m_step;
		CollisionContact* m_contacts;
	} result;
	result.m_step = p_stepSeconds;
	LegoS32 contactCount = SolveContacts(&result.m_contacts);
	g_contactCacheController.ResetContacts();
	g_contactCacheController.CollectContacts(result.m_contacts, contactCount);

	for (LegoS32 i = 0; i < g_gameObjectsByTypeCount; i++) {
		GameObject* object = g_gameObjectsByType[i];
		if (!CanMove(object)) {
			continue;
		}
		GameLifecycleComponent* component = object->m_lifecycle0x54;
		if (component != NULL && component->m_vtable->m_isActive(component, NULL, 0)) {
			LegoU8* state = (LegoU8*) component;
			if (state[0xa1] != 0) {
				component->m_owner->m_flags |= 2;
				state[0xa1] = 0;
			}
			if (!CanMove(object)) {
				continue;
			}
			if (state[0xa0] != 0) {
				component->m_owner->m_flags &= ~2;
				state[0xa0] = 0;
			}
		}
		object->m_vtable->m_value0x24(object, NULL);
	}

	LegoS32 boundary = g_gameObjectsByTypeCount - 1;
	for (; boundary >= 0; boundary--) {
		GameObject* object = g_gameObjectsByType[boundary];
		if (object->m_vtable->m_value0x38(object, NULL) == 1) {
			break;
		}
		if (CanMove(object)) {
			object->m_vtable->m_value0x20(object, NULL);
		}
	}

	LegoS32 remaining = boundary;
	for (LegoU32 pass = 0; pass < 4; pass++) {
		remaining = boundary;
		for (; remaining >= 0; remaining--) {
			GameObject* object = g_gameObjectsByType[remaining];
			if (object->m_vtable->m_value0x38(object, NULL) != 1) {
				break;
			}
			if (CanMove(object)) {
				object->m_vtable->m_value0x20(object, NULL);
			}
		}
	}
	for (; remaining >= 0; remaining--) {
		GameObject* object = g_gameObjectsByType[remaining];
		if (CanMove(object)) {
			object->m_vtable->m_value0x20(object, NULL);
		}
	}
	GetInitializedSimulationScheduler();
	// Retail's corresponding 0x00489970(frame, 20) is also a no-effect call.
}

// STUB: LEGORACERS2 0x004e6aa0
void GameManager::RefreshCollisionStates()
{
	GameObject** current = g_gameObjectsWithActiveComponents;
	GameObject** end = current + g_gameObjectsWithActiveComponentsCount;
	while (current != end) {
		(*current)->RefreshCollisionState();
		++current;
	}
}
