#include "collisioncontacts.h"
#include "collisionservice.h"
#include "contactimpulse.h"

// GLOBAL: LEGORACERS2 0x005368dc
LegoFloat g_contactResponseThreshold = 0.3f;

namespace
{
struct ContactLifecycleDispatch {
	typedef LegoBool32(__fastcall* Predicate)(PhysicalLifecycleState*, void*, LegoU32);
	typedef void(__fastcall* ContactAction)(PhysicalLifecycleState*, void*, CollisionContact*);
	typedef const Vector3*(__fastcall* Position)(PhysicalLifecycleState*, void*);
	typedef LegoBool32(__fastcall* Filter)(PhysicalLifecycleState*, void*, CollisionContact*);
	typedef void(__fastcall* ImpulseAction)(PhysicalLifecycleState*, void*, CollisionContact*, ContactImpulseBody*);
	Predicate m_isActive;
	undefined m_unknown0x04[0x10 - 0x04];
	ContactAction m_contact;
	Position m_position;
	undefined m_unknown0x18[0x34 - 0x18];
	Filter m_filter;
	undefined m_unknown0x38[4];
	ImpulseAction m_impulse;
};
DECOMP_SIZE_ASSERT(ContactLifecycleDispatch, 0x40);
DECOMP_STATIC_ASSERT(offsetof(ContactLifecycleDispatch, m_filter) == 0x34);

const ContactLifecycleDispatch* ContactDispatch(PhysicalLifecycleState* p_body)
{
	return (const ContactLifecycleDispatch*) p_body->m_base.m_vtable;
}

void SnapshotImpulseBody(ContactImpulseBody& p_snapshot, PhysicalLifecycleState* p_body)
{
	p_snapshot.m_linearMomentum = &p_body->m_linearMomentum;
	p_snapshot.m_angularMomentum = &p_body->m_angularMomentum;
	p_snapshot.m_inverseMass = p_body->m_inverseMass;
	p_snapshot.m_inverseInertia = &p_body->m_inverseInertiaTensor;
	p_snapshot.m_restitution = p_body->m_contactResponse;
}

void ApplyImpulseBody(PhysicalLifecycleState* p_body, const ContactImpulseBody& p_snapshot)
{
	p_body->m_linearMomentum.m_x += p_snapshot.m_linearImpulse.m_x;
	p_body->m_linearMomentum.m_y += p_snapshot.m_linearImpulse.m_y;
	p_body->m_linearMomentum.m_z += p_snapshot.m_linearImpulse.m_z;
	p_body->m_angularMomentum.m_x += p_snapshot.m_angularImpulse.m_x;
	p_body->m_angularMomentum.m_y += p_snapshot.m_angularImpulse.m_y;
	p_body->m_angularMomentum.m_z += p_snapshot.m_angularImpulse.m_z;
	p_body->UpdateAndClampVelocities();
}
} // namespace

// STUB: LEGORACERS2 0x004e6ad0
LegoS32 GameManager::SolveContacts(CollisionContact** p_contacts)
{
	if (g_collisionProxyService == NULL) {
		return 0;
	}
	LegoS32 count =
		(LegoS32)
			g_collisionProxyService->m_vtable->m_prepareQuery(g_collisionProxyService, NULL, (LegoU32*) p_contacts);
	for (LegoS32 i = 0; i < count; ++i) {
		CollisionContact& contact = (*p_contacts)[i];
		PhysicalLifecycleState* first = (PhysicalLifecycleState*) contact.m_first;
		PhysicalLifecycleState* second = (PhysicalLifecycleState*) contact.m_second;
		if (!ContactDispatch(first)->m_isActive(first, NULL, 1)) {
			contact.SwapParticipants(NULL, contact.m_first, contact.m_second);
			second = first;
			first = (PhysicalLifecycleState*) contact.m_first;
		}
		contact.m_secondOffset = *ContactDispatch(first)->m_position(first, NULL);
		const Vector3* secondPosition = ContactDispatch(second)->m_position(second, NULL);
		contact.m_secondOffset.m_x -= secondPosition->m_x;
		contact.m_secondOffset.m_y -= secondPosition->m_y;
		contact.m_secondOffset.m_z -= secondPosition->m_z;
		contact.m_secondOffset.m_x += contact.m_point.m_x;
		contact.m_secondOffset.m_y += contact.m_point.m_y;
		contact.m_secondOffset.m_z += contact.m_point.m_z;
		contact.m_secondOffset.m_x += contact.m_penetration * contact.m_separationAxis.m_x;
		contact.m_secondOffset.m_y += contact.m_penetration * contact.m_separationAxis.m_y;
		contact.m_secondOffset.m_z += contact.m_penetration * contact.m_separationAxis.m_z;
	}
	for (LegoU32 pass = 0; pass < 2; ++pass) {
		bool applied = false;
		for (LegoS32 i = 0; i < count; ++i) {
			CollisionContact& contact = (*p_contacts)[i];
			PhysicalLifecycleState* first = (PhysicalLifecycleState*) contact.m_first;
			PhysicalLifecycleState* second = (PhysicalLifecycleState*) contact.m_second;
			contact.m_relativeVelocity.Cross(&first->m_worldAngularVelocity, &contact.m_point);
			contact.m_relativeVelocity.m_x += first->m_linearVelocity.m_x;
			contact.m_relativeVelocity.m_y += first->m_linearVelocity.m_y;
			contact.m_relativeVelocity.m_z += first->m_linearVelocity.m_z;
			if (ContactDispatch(second)->m_isActive(second, NULL, 0)) {
				Vector3 velocity;
				velocity.Cross(&second->m_worldAngularVelocity, &contact.m_secondOffset);
				contact.m_relativeVelocity.m_x -= velocity.m_x + second->m_linearVelocity.m_x;
				contact.m_relativeVelocity.m_y -= velocity.m_y + second->m_linearVelocity.m_y;
				contact.m_relativeVelocity.m_z -= velocity.m_z + second->m_linearVelocity.m_z;
			}
			LegoFloat normalVelocity = contact.m_normal.m_x * contact.m_relativeVelocity.m_x +
									   contact.m_normal.m_y * contact.m_relativeVelocity.m_y +
									   contact.m_normal.m_z * contact.m_relativeVelocity.m_z;
			if (g_contactResponseThreshold * -0.5f < normalVelocity) {
				contact.m_flags = normalVelocity < g_contactResponseThreshold ? 2 : 0x10;
			}
			else {
				contact.m_flags = 1;
				first->m_base.m_stateFlags &= ~2;
				second->m_base.m_stateFlags &= ~2;
			}
			LegoBool32 firstFiltered = ContactDispatch(first)->m_filter(first, NULL, &contact);
			LegoBool32 secondFiltered = ContactDispatch(second)->m_filter(second, NULL, &contact);
			if (firstFiltered || secondFiltered) {
				contact.m_flags &= 0x40;
				continue;
			}
			ContactDispatch(first)->m_contact(first, NULL, &contact);
			ContactDispatch(second)->m_contact(second, NULL, &contact);
			if (contact.m_flags != 1) {
				continue;
			}
			ContactImpulseBody firstBody;
			SnapshotImpulseBody(firstBody, first);
			applied = true;
			if (!ContactDispatch(second)->m_isActive(second, NULL, 0)) {
				SeparateContactFromStaticBody(first, &contact);
				firstBody.m_restitution *= second->m_contactResponse;
				ComputeSingleBodyContactImpulse(&firstBody, &contact);
				ContactDispatch(first)->m_impulse(first, NULL, &contact, &firstBody);
			}
			else if (!ContactDispatch(second)->m_isActive(second, NULL, 1)) {
				SeparateContactFromStaticBody(first, &contact);
				ContactImpulseBody secondBody;
				SnapshotImpulseBody(secondBody, second);
				ComputeTwoBodyContactImpulse(&firstBody, &secondBody, &contact);
				ContactDispatch(first)->m_impulse(first, NULL, &contact, &firstBody);
				ContactDispatch(second)->m_impulse(second, NULL, &contact, &secondBody);
			}
			else {
				SeparateContactBodies(first, second, &contact);
				ContactImpulseBody secondBody;
				SnapshotImpulseBody(secondBody, second);
				ComputeTwoBodyContactImpulse(&firstBody, &secondBody, &contact);
				ContactDispatch(first)->m_impulse(first, NULL, &contact, &firstBody);
				ContactDispatch(second)->m_impulse(second, NULL, &contact, &secondBody);
				ApplyImpulseBody(second, secondBody);
			}
			ApplyImpulseBody(first, firstBody);
			first->m_base.m_owner->m_flags &= ~2;
			second->m_base.m_owner->m_flags &= ~2;
		}
		if (!applied) {
			return count;
		}
	}
	return count;
}
