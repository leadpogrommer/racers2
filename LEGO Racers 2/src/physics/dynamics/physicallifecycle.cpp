#include "physics/dynamics/physicallifecycle.h"

#include <math.h>

extern LegoFloat g_maximumLinearSpeedSquared;
extern LegoFloat g_maximumAngularSpeedSquared;

static void ScalePhysicsVector(Vector3* p_vector, LegoFloat p_scale)
{
	p_vector->m_x *= p_scale;
	p_vector->m_y *= p_scale;
	p_vector->m_z *= p_scale;
}

// FUNCTION: LEGORACERS2 0x004a6480
void PhysicalLifecycleState::RebuildInverseInertiaTensor()
{
	Matrix3 scaled;
	scaled.FUN_004a52c0(&m_inverseInertia.m_x, &m_rotation);
	m_inverseInertiaTensor.FUN_004a4ea0(&m_rotation, &scaled);
}

// STUB: LEGORACERS2 0x004a64c0
void PhysicalLifecycleState::UpdateAndClampVelocities()
{
	m_linearVelocity.m_x = m_inverseMass * m_linearMomentum.m_x;
	m_linearVelocity.m_y = m_inverseMass * m_linearMomentum.m_y;
	m_linearVelocity.m_z = m_inverseMass * m_linearMomentum.m_z;
	m_worldAngularVelocity = m_angularMomentum;
	m_worldAngularVelocity.FUN_004a4e00(&m_inverseInertiaTensor);
	LegoFloat length = m_linearVelocity.m_z * m_linearVelocity.m_z + m_linearVelocity.m_y * m_linearVelocity.m_y +
					   m_linearVelocity.m_x * m_linearVelocity.m_x;
	if (length > g_maximumLinearSpeedSquared) {
		LegoFloat scale = (LegoFloat) sqrt(g_maximumLinearSpeedSquared / length);
		ScalePhysicsVector(&m_linearMomentum, scale);
		ScalePhysicsVector(&m_linearVelocity, scale);
	}
	length = m_worldAngularVelocity.m_z * m_worldAngularVelocity.m_z +
			 m_worldAngularVelocity.m_y * m_worldAngularVelocity.m_y +
			 m_worldAngularVelocity.m_x * m_worldAngularVelocity.m_x;
	// Both retail comparisons skip clamping when the comparison is unordered.
	if (length > g_maximumAngularSpeedSquared) {
		LegoFloat scale = (LegoFloat) sqrt(g_maximumAngularSpeedSquared / length);
		ScalePhysicsVector(&m_angularMomentum, scale);
		ScalePhysicsVector(&m_worldAngularVelocity, scale);
	}
}

// STUB: LEGORACERS2 0x004a6c30
void __fastcall ClearLifecycleParent(GameLifecycleComponent* p_lifecycle, void*, LegoU32 p_flag)
{
	PhysicalLifecycleState* state = (PhysicalLifecycleState*) p_lifecycle;
	if (p_flag == 0) {
		state->m_position = *state->GetTransformDispatch()->m_position(state, NULL);
		state->m_orientation = *state->GetTransformDispatch()->m_orientation(state, NULL);
		state->m_rotation.FUN_004a4d30(&state->m_orientation.m_x);
		Vector3 zero = {0, 0, 0};
		state->m_torque = zero;
		state->m_force = zero;
		state->m_parentValue = state->m_savedParentValue;
		Vector3 velocity;
		state->GetParentVelocityAtOffset(&state->m_parentOffset, &velocity);
		state->m_linearMomentum.m_x = velocity.m_x * state->m_mass;
		state->m_linearMomentum.m_y = velocity.m_y * state->m_mass;
		state->m_linearMomentum.m_z = velocity.m_z * state->m_mass;
		state->m_angularMomentum = zero;
		state->RebuildInverseInertiaTensor();
		PhysicalLifecycleState* parent = state->GetTransformDispatch()->m_parent(state, NULL);
		if (parent->GetTransformDispatch()->m_isActive(parent, NULL, 0) == FALSE) {
			state->m_worldAngularVelocity = zero;
		}
		else {
			state->m_worldAngularVelocity = parent->m_worldAngularVelocity;
		}
		Matrix3 inverse;
		inverse.FUN_004a5150(&state->m_inverseInertiaTensor);
		state->m_angularMomentum = state->m_worldAngularVelocity;
		state->m_angularMomentum.FUN_004a4e00(&inverse);
		state->UpdateAndClampVelocities();
	}
	state->m_parent = NULL;
}

// STUB: LEGORACERS2 0x004a6f00
void PhysicalLifecycleState::GetParentVelocityAtOffset(const Vector3* p_offset, Vector3* p_result)
{
	PhysicalLifecycleState* parent = m_parent;
	if (parent->GetTransformDispatch()->m_isActive(parent, NULL, 0) == FALSE) {
		p_result->m_x = p_result->m_y = p_result->m_z = 0;
		return;
	}
	parent = m_parent;
	Vector3 offset = *p_offset;
	offset.FUN_004a4e00(parent->GetTransformDispatch()->m_rotation(parent, NULL));
	LegoFloat x = offset.m_z * parent->m_worldAngularVelocity.m_y - offset.m_y * parent->m_worldAngularVelocity.m_z;
	LegoFloat y = offset.m_x * parent->m_worldAngularVelocity.m_z - offset.m_z * parent->m_worldAngularVelocity.m_x;
	p_result->m_z = offset.m_y * parent->m_worldAngularVelocity.m_x - offset.m_x * parent->m_worldAngularVelocity.m_y;
	p_result->m_x = x;
	p_result->m_y = y;
	p_result->m_x = x + parent->m_linearVelocity.m_x;
	p_result->m_y = y + parent->m_linearVelocity.m_y;
	p_result->m_z = parent->m_linearVelocity.m_z + p_result->m_z;
}
