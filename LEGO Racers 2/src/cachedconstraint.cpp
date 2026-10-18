#include "cachedconstraint.h"

#include "collisioncontacts.h"

namespace
{
bool CanApplyConstraint(const PhysicalLifecycleState* p_body)
{
	LegoU32 flags = p_body->m_base.m_owner->m_flags;
	return (flags & 0x12) == 0;
}
} // namespace

CachedConstraint::~CachedConstraint()
{
	m_vtable = &g_constraintBaseDispatch;
}

// STUB: LEGORACERS2 0x004cd7c0
CachedConstraint::CachedConstraint()
{
	for (LegoS32 i = 0; i < 2; ++i) {
		m_endpoints[i].m_body = NULL;
		m_endpoints[i].m_localPosition.m_x = 0;
		m_endpoints[i].m_localPosition.m_y = 0;
		m_endpoints[i].m_localPosition.m_z = 0;
		m_endpoints[i].m_fixed = 1;
	}
	m_force.m_x = 0;
	m_force.m_y = 0;
	m_force.m_z = 0;
	m_vtable = &g_constraintBaseDispatch;
	m_endpoints[0].SetWorldBinding(NULL);
	m_endpoints[1].SetWorldBinding(NULL);
}

// STUB: LEGORACERS2 0x004cd820
LegoBool32 CachedConstraint::IsInactive()
{
	for (LegoS32 i = 0; i < 2; ++i) {
		PhysicalLifecycleState* body = m_endpoints[i].m_body;
		if (body->GetTransformDispatch()->m_isActive(body, NULL, 0) && CanApplyConstraint(body)) {
			return FALSE;
		}
	}
	return TRUE;
}

// FUNCTION: LEGORACERS2 0x004cd870
void CachedConstraint::GetPositionDifference(Vector3* p_result)
{
	Vector3 first;
	m_endpoints[0].GetWorldPosition(&first);
	m_endpoints[1].GetWorldPosition(p_result);
	p_result->m_x -= first.m_x;
	p_result->m_y -= first.m_y;
	p_result->m_z -= first.m_z;
}

// FUNCTION: LEGORACERS2 0x004cd8c0
void CachedConstraint::GetVelocityDifference(Vector3* p_result)
{
	Vector3 second;
	m_endpoints[0].GetPointVelocity(p_result);
	m_endpoints[1].GetPointVelocity(&second);
	p_result->m_x -= second.m_x;
	p_result->m_y -= second.m_y;
	p_result->m_z -= second.m_z;
}

// STUB: LEGORACERS2 0x004cd910
void CachedConstraint::ApplyForces(LegoU32 p_reverse)
{
	if (IsInactive()) {
		return;
	}
	bool applied = false;
	for (LegoS32 index = 0; index < 2; ++index) {
		ConstraintEndpoint& endpoint = m_endpoints[index];
		PhysicalLifecycleState* body = endpoint.m_body;
		if (endpoint.m_fixed == 0 && CanApplyConstraint(body)) {
			applied = true;
			Vector3 force = m_force;
			if (index == 1) {
				force.m_x *= -1.0f;
				force.m_y *= -1.0f;
				force.m_z *= -1.0f;
			}
			if (p_reverse) {
				force.m_x *= -1.0f;
				force.m_y *= -1.0f;
				force.m_z *= -1.0f;
			}
			body->m_force.m_x += force.m_x;
			body->m_force.m_y += force.m_y;
			body->m_force.m_z += force.m_z;
			Vector3 torque;
			torque.Cross(&endpoint.m_worldOffset, &force);
			body->m_torque.m_x += torque.m_x;
			body->m_torque.m_y += torque.m_y;
			body->m_torque.m_z += torque.m_z;
		}
	}
	if (applied) {
		for (LegoS32 index = 0; index < 2; ++index) {
			ConstraintEndpoint& endpoint = m_endpoints[index];
			if (endpoint.m_fixed == 0 && !CanApplyConstraint(endpoint.m_body)) {
				*(LegoU8*) &endpoint.m_body->m_unk0xa0 = 1;
			}
		}
	}
	if (m_endpoints[0].m_fixed == 0) {
		if (m_endpoints[1].m_fixed != 0) {
			m_endpoints[0].m_body->UpdateSleepState();
		}
	}
	else if (m_endpoints[1].m_fixed == 0) {
		m_endpoints[1].m_body->UpdateSleepState();
	}
}

// STUB: LEGORACERS2 0x004cdb00
void CachedConstraint::CopyContact(const CollisionContact* p_contact)
{
	for (LegoS32 index = 0; index < 2; ++index) {
		PhysicalLifecycleState* body =
			(PhysicalLifecycleState*) (index == 0 ? p_contact->m_first : p_contact->m_second);
		Vector3 point = index == 0 ? p_contact->m_point : p_contact->m_secondOffset;
		PhysicalLifecycleState* parent = body->GetTransformDispatch()->m_parent(body, NULL);
		while (parent != NULL) {
			Vector3 offset;
			body->GetWorldParentOffset(&offset);
			point.m_x += offset.m_x;
			point.m_y += offset.m_y;
			point.m_z += offset.m_z;
			body = parent;
			parent = body->GetTransformDispatch()->m_parent(body, NULL);
		}
		if (!body->GetTransformDispatch()->m_isActive(body, NULL, 0)) {
			const Vector3* position = body->GetTransformDispatch()->m_position(body, NULL);
			point.m_x += position->m_x;
			point.m_y += position->m_y;
			point.m_z += position->m_z;
		}
		else {
			point.FUN_004a4e50(body->GetTransformDispatch()->m_rotation(body, NULL));
		}
		ConstraintEndpointBinding binding;
		binding.m_body = body;
		binding.m_position = point;
		m_endpoints[index].SetBinding(&binding, 0);
	}
}

// FUNCTION: LEGORACERS2 0x004cdc10
void CachedConstraint::GetAccelerationDifference(Vector3* p_result)
{
	Vector3 second;
	m_endpoints[0].GetPointAcceleration(p_result);
	m_endpoints[1].GetPointAcceleration(&second);
	p_result->m_x -= second.m_x;
	p_result->m_y -= second.m_y;
	p_result->m_z -= second.m_z;
}

// STUB: LEGORACERS2 0x004cdc60
void CachedConstraint::ComputeNormalForce(Vector3* p_result, const Vector3* p_acceleration, const Vector3* p_normal)
{
	*p_result = *p_normal;
	p_result->FUN_004a4e00(&m_effectiveMass);
	LegoFloat scale = (p_acceleration->m_x * p_normal->m_x + p_acceleration->m_y * p_normal->m_y +
					   p_acceleration->m_z * p_normal->m_z) /
					  (p_result->m_x * p_normal->m_x + p_result->m_z * p_normal->m_z + p_result->m_y * p_normal->m_y);
	p_result->m_x = scale * p_normal->m_x;
	p_result->m_y = scale * p_normal->m_y;
	p_result->m_z = scale * p_normal->m_z;
}

// FUNCTION: LEGORACERS2 0x004cdd00
void CachedConstraint::RebuildEffectiveMassMatrix(LegoU32 p_invert)
{
	Matrix3 second;
	m_endpoints[0].GetEffectiveMassMatrix(&m_effectiveMass);
	m_endpoints[1].GetEffectiveMassMatrix(&second);
	m_effectiveMass.FUN_004a5110(&second);
	if (p_invert) {
		m_inverseEffectiveMass.FUN_004a5150(&m_effectiveMass);
	}
}
