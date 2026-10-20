#include "physics/dynamics/cachedconstraint.h"

// STUB: LEGORACERS2 0x004cdd70
void ConstraintEndpoint::SetBinding(const ConstraintEndpointBinding* p_binding, LegoU32 p_fixed)
{
	if (p_binding == NULL) {
		m_body = NULL;
		m_fixed = 1;
		m_localPosition.m_x = 0;
		m_localPosition.m_y = 0;
		m_localPosition.m_z = 0;
		return;
	}
	m_body = p_binding->m_body;
	m_localPosition = p_binding->m_position;
	if (p_fixed) {
		m_fixed = 1;
	}
	else {
		m_fixed = m_body->GetTransformDispatch()->m_isActive(m_body, NULL, 0) == 0;
	}
}

// STUB: LEGORACERS2 0x004cde00
void ConstraintEndpoint::SetWorldBinding(const ConstraintEndpointBinding* p_binding)
{
	if (p_binding != NULL) {
		SetBinding(p_binding, 0);
		if (m_fixed) {
			m_localPosition.FUN_004a4e00(m_body->GetTransformDispatch()->m_rotation(m_body, NULL));
			const Vector3* position = m_body->GetTransformDispatch()->m_position(m_body, NULL);
			m_localPosition.m_x += position->m_x;
			m_localPosition.m_y += position->m_y;
			m_localPosition.m_z += position->m_z;
		}
	}
}

// FUNCTION: LEGORACERS2 0x004cde60
void ConstraintEndpoint::GetWorldPosition(Vector3* p_result)
{
	if (m_fixed) {
		*p_result = m_localPosition;
	}
	else {
		*p_result = m_worldOffset;
		const Vector3* position = m_body->GetTransformDispatch()->m_position(m_body, NULL);
		p_result->m_x += position->m_x;
		p_result->m_y += position->m_y;
		p_result->m_z += position->m_z;
	}
}

// STUB: LEGORACERS2 0x004cdec0
void ConstraintEndpoint::UpdateWorldOffset()
{
	m_worldOffset = m_localPosition;
	if (!m_fixed) {
		m_worldOffset.FUN_004a4e00(m_body->GetTransformDispatch()->m_rotation(m_body, NULL));
	}
}

// STUB: LEGORACERS2 0x004cdf10
void ConstraintEndpoint::GetPointVelocity(Vector3* p_result)
{
	if (m_fixed) {
		p_result->m_x = 0;
		p_result->m_y = 0;
		p_result->m_z = 0;
	}
	else {
		p_result->Cross(&m_body->m_worldAngularVelocity, &m_worldOffset);
		p_result->m_x += m_body->m_linearVelocity.m_x;
		p_result->m_y += m_body->m_linearVelocity.m_y;
		p_result->m_z += m_body->m_linearVelocity.m_z;
	}
}

// STUB: LEGORACERS2 0x004cdff0
void ConstraintEndpoint::UpdatePointAcceleration()
{
	if (m_fixed) {
		m_acceleration.m_x = 0;
		m_acceleration.m_y = 0;
		m_acceleration.m_z = 0;
		return;
	}
	Vector3 velocity;
	velocity.Cross(&m_body->m_worldAngularVelocity, &m_worldOffset);
	m_acceleration.Cross(&m_body->m_worldAngularVelocity, &velocity);
	Vector3 gyroscopic;
	gyroscopic.Cross(&m_body->m_angularMomentum, &m_body->m_worldAngularVelocity);
	gyroscopic.FUN_004a4e00(&m_body->m_inverseInertiaTensor);
	Vector3 angular;
	angular.Cross(&gyroscopic, &m_worldOffset);
	m_acceleration.m_x += angular.m_x;
	m_acceleration.m_y += angular.m_y;
	m_acceleration.m_z += angular.m_z;
}

// STUB: LEGORACERS2 0x004ce1c0
void ConstraintEndpoint::GetPointAcceleration(Vector3* p_result)
{
	if (m_fixed == 0 && (m_body->m_base.m_owner->m_flags & 0x12) == 0) {
		p_result->m_x = m_body->m_inverseMass * m_body->m_force.m_x;
		p_result->m_y = m_body->m_inverseMass * m_body->m_force.m_y;
		p_result->m_z = m_body->m_inverseMass * m_body->m_force.m_z;
		p_result->m_x += m_acceleration.m_x;
		p_result->m_y += m_acceleration.m_y;
		p_result->m_z += m_acceleration.m_z;
		Vector3 angular = m_body->m_torque;
		angular.FUN_004a4e00(&m_body->m_inverseInertiaTensor);
		Vector3 acceleration;
		acceleration.Cross(&angular, &m_worldOffset);
		p_result->m_x += acceleration.m_x;
		p_result->m_y += acceleration.m_y;
		p_result->m_z += acceleration.m_z;
	}
	else {
		p_result->m_x = 0;
		p_result->m_y = 0;
		p_result->m_z = 0;
	}
}

// STUB: LEGORACERS2 0x004ce300
void ConstraintEndpoint::GetEffectiveMassMatrix(Matrix3* p_result)
{
	if (m_fixed) {
		for (LegoU32 row = 0; row < 3; ++row) {
			for (LegoU32 column = 0; column < 3; ++column) {
				p_result->m_values[row][column] = 0;
			}
		}
	}
	else {
		Vector3 offset = m_worldOffset;
		Matrix3 moment;
		moment.FUN_004a5080(&offset.m_x);
		p_result->FUN_004a4ea0(&m_body->m_inverseInertiaTensor, &moment);
		p_result->m_values[0][0] += m_body->m_inverseMass;
		p_result->m_values[1][1] += m_body->m_inverseMass;
		p_result->m_values[2][2] += m_body->m_inverseMass;
	}
}
