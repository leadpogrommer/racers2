#include "contactimpulse.h"

namespace
{
LegoFloat DotContactVectors(const Vector3& p_first, const Vector3& p_second)
{
	return p_first.m_z * p_second.m_z + p_first.m_y * p_second.m_y + p_first.m_x * p_second.m_x;
}

void ClearContactImpulses(ContactImpulseBody* p_body)
{
	Vector3 zero = {0, 0, 0};
	p_body->m_linearImpulse = zero;
	p_body->m_angularImpulse = zero;
}

LegoFloat ComputeContactAngularDenominator(const Vector3* p_offset, const Vector3* p_normal, const Matrix3* p_inertia)
{
	Vector3 cross;
	cross.Cross(p_offset, p_normal);
	cross.FUN_004a4e00(p_inertia);
	Vector3 response;
	response.Cross(&cross, p_offset);
	return DotContactVectors(response, *p_normal);
}

void StoreContactImpulse(
	ContactImpulseBody* p_body,
	const Vector3& p_normal,
	const Vector3* p_offset,
	LegoFloat p_impulse
)
{
	p_body->m_linearImpulse = p_normal;
	p_body->m_linearImpulse.m_x *= p_impulse;
	p_body->m_linearImpulse.m_y *= p_impulse;
	p_body->m_linearImpulse.m_z *= p_impulse;
	p_body->m_angularImpulse.Cross(p_offset, &p_body->m_linearImpulse);
}
} // namespace

// STUB: LEGORACERS2 0x004fc050
void ComputeSingleBodyContactImpulse(ContactImpulseBody* p_body, const CollisionContact* p_contact)
{
	ClearContactImpulses(p_body);
	LegoFloat velocity = DotContactVectors(p_contact->m_normal, p_contact->m_relativeVelocity);
	if (!(velocity >= 0)) {
		LegoFloat numerator = -((p_body->m_restitution + 1.0f) * velocity);
		LegoFloat denominator =
			ComputeContactAngularDenominator(&p_contact->m_point, &p_contact->m_normal, p_body->m_inverseInertia) +
			p_body->m_inverseMass;
		StoreContactImpulse(p_body, p_contact->m_normal, &p_contact->m_point, numerator / denominator);
	}
}

// STUB: LEGORACERS2 0x004fc1f0
void ComputeTwoBodyContactImpulse(
	ContactImpulseBody* p_first,
	ContactImpulseBody* p_second,
	const CollisionContact* p_contact
)
{
	ClearContactImpulses(p_first);
	ClearContactImpulses(p_second);
	LegoFloat velocity = DotContactVectors(p_contact->m_normal, p_contact->m_relativeVelocity);
	if (!(velocity >= 0)) {
		LegoFloat restitution = p_first->m_restitution * p_second->m_restitution + 1.0f;
		LegoFloat firstDenominator =
			ComputeContactAngularDenominator(&p_contact->m_point, &p_contact->m_normal, p_first->m_inverseInertia);
		LegoFloat secondDenominator = ComputeContactAngularDenominator(
			&p_contact->m_secondOffset,
			&p_contact->m_normal,
			p_second->m_inverseInertia
		);
		LegoFloat denominator = secondDenominator + p_second->m_inverseMass + firstDenominator + p_first->m_inverseMass;
		LegoFloat impulse = -(restitution * velocity) / denominator;
		StoreContactImpulse(p_first, p_contact->m_normal, &p_contact->m_point, impulse);
		p_second->m_linearImpulse.m_x -= p_first->m_linearImpulse.m_x;
		p_second->m_linearImpulse.m_y -= p_first->m_linearImpulse.m_y;
		p_second->m_linearImpulse.m_z -= p_first->m_linearImpulse.m_z;
		Vector3 angular;
		angular.Cross(&p_contact->m_secondOffset, &p_first->m_linearImpulse);
		p_second->m_angularImpulse.m_x -= angular.m_x;
		p_second->m_angularImpulse.m_y -= angular.m_y;
		p_second->m_angularImpulse.m_z -= angular.m_z;
	}
}

// STUB: LEGORACERS2 0x004fc4e0
LegoFloat ComputePenetrationCorrectionScale(LegoFloat p_speed)
{
	LegoFloat speed;
	if (p_speed >= 5.0f) {
		speed = p_speed <= 20.0f ? p_speed : 20.0f;
	}
	else {
		speed = 5.0f;
	}
	return (speed - 5.0f) * 0.06666667014360428f * 0.3999999761581421f + 0.30000001192092896f;
}

// FUNCTION: LEGORACERS2 0x004fc530
void ComputeContactSeparationWeights(
	LegoFloat p_firstMass,
	LegoFloat p_secondMass,
	LegoFloat* p_first,
	LegoFloat* p_second
)
{
	LegoFloat mass = p_firstMass + p_secondMass;
	if (!(mass >= 1.0f)) {
		*p_second = 0.5f;
		*p_first = 0.5f;
	}
	else {
		*p_first = p_secondMass / mass;
		*p_second = p_firstMass / mass;
	}
}

// STUB: LEGORACERS2 0x004fc580
void SeparateContactBodies(
	PhysicalLifecycleState* p_first,
	PhysicalLifecycleState* p_second,
	const CollisionContact* p_contact
)
{
	if ((p_contact->m_flags & 2) != 0) {
		return;
	}
	LegoFloat firstWeight;
	LegoFloat secondWeight;
	ComputeContactSeparationWeights(p_first->m_mass, p_second->m_mass, &firstWeight, &secondWeight);
	Vector3 first = *p_first->GetTransformDispatch()->m_position(p_first, NULL);
	Vector3 second = *p_second->GetTransformDispatch()->m_position(p_second, NULL);
	LegoFloat distance = firstWeight * p_contact->m_penetration * 0.5f;
	first.m_x = distance * p_contact->m_separationAxis.m_x + first.m_x;
	first.m_y = distance * p_contact->m_separationAxis.m_y + first.m_y;
	first.m_z = distance * p_contact->m_separationAxis.m_z + first.m_z;
	distance = secondWeight * p_contact->m_penetration * -0.5f;
	second.m_x = distance * p_contact->m_separationAxis.m_x + second.m_x;
	second.m_y = distance * p_contact->m_separationAxis.m_y + second.m_y;
	second.m_z = distance * p_contact->m_separationAxis.m_z + second.m_z;
	p_first->GetTransformDispatch()->m_setPosition(p_first, NULL, &first);
	p_second->GetTransformDispatch()->m_setPosition(p_second, NULL, &second);
}

// STUB: LEGORACERS2 0x004fc680
void SeparateContactFromStaticBody(PhysicalLifecycleState* p_body, const CollisionContact* p_contact)
{
	if ((p_contact->m_flags & 2) != 0 || !(DotContactVectors(p_contact->m_separationAxis, p_contact->m_normal) >= 0)) {
		return;
	}
	LegoFloat speed = -DotContactVectors(p_contact->m_separationAxis, p_contact->m_relativeVelocity);
	LegoFloat correction = ComputePenetrationCorrectionScale(speed);
	Vector3 position = *p_body->GetTransformDispatch()->m_position(p_body, NULL);
	LegoFloat distance = correction * p_contact->m_penetration;
	position.m_x = distance * p_contact->m_separationAxis.m_x + position.m_x;
	position.m_y = distance * p_contact->m_separationAxis.m_y + position.m_y;
	position.m_z = distance * p_contact->m_separationAxis.m_z + position.m_z;
	p_body->GetTransformDispatch()->m_setPosition(p_body, NULL, &position);
}
