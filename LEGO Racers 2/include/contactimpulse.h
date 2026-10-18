#ifndef LR2_CONTACTIMPULSE_H
#define LR2_CONTACTIMPULSE_H

#include "collisioncontacts.h"
#include "physicallifecycle.h"

struct ContactImpulseBody {
	Vector3* m_linearMomentum;
	Vector3* m_angularMomentum;
	LegoFloat m_inverseMass;
	const Matrix3* m_inverseInertia;
	LegoFloat m_restitution;
	Vector3 m_linearImpulse;
	Vector3 m_angularImpulse;
};
DECOMP_SIZE_ASSERT(ContactImpulseBody, 0x2c);

void ComputeSingleBodyContactImpulse(ContactImpulseBody* p_body, const CollisionContact* p_contact);
void ComputeTwoBodyContactImpulse(
	ContactImpulseBody* p_first,
	ContactImpulseBody* p_second,
	const CollisionContact* p_contact
);
LegoFloat ComputePenetrationCorrectionScale(LegoFloat p_speed);
void ComputeContactSeparationWeights(
	LegoFloat p_firstMass,
	LegoFloat p_secondMass,
	LegoFloat* p_first,
	LegoFloat* p_second
);
void SeparateContactBodies(
	PhysicalLifecycleState* p_first,
	PhysicalLifecycleState* p_second,
	const CollisionContact* p_contact
);
void SeparateContactFromStaticBody(PhysicalLifecycleState* p_body, const CollisionContact* p_contact);

#endif // LR2_CONTACTIMPULSE_H
