#include "physicallifecycle.h"

extern LegoFloat g_currentSimulationStep;

// STUB: LEGORACERS2 0x004a6620
LegoU32 __fastcall ApplyPhysicalLifecycleDampingAndGravity(PhysicalLifecycleState* p_state, void*)
{
	// Retail 0x473140 on the parent path is the shared zero-return body.
	if (p_state->m_parent != NULL) {
		return 0;
	}
	LegoFloat scale = g_physicalLifecycleNormalizedStep;
	p_state->m_torque.m_x = scale * p_state->m_angularMomentum.m_x;
	p_state->m_torque.m_y = scale * p_state->m_angularMomentum.m_y;
	p_state->m_torque.m_z = scale * p_state->m_angularMomentum.m_z;
	scale = g_physicalLifecycleNormalizedStep;
	p_state->m_force.m_x = scale * p_state->m_linearMomentum.m_x;
	p_state->m_force.m_y = scale * p_state->m_linearMomentum.m_y;
	p_state->m_force.m_z = scale * p_state->m_linearMomentum.m_z;
	p_state->m_force.m_y = g_physicalLifecycleStep * p_state->m_mass + p_state->m_force.m_y;
	if ((p_state->m_base.m_stateFlags & 1) != 0) {
		Vector3 zero = {0.0f, 0.0f, 0.0f};
		p_state->m_torque = zero;
		p_state->m_force = zero;
	}
	return 0;
}

// STUB: LEGORACERS2 0x004a6710
void __fastcall ApplyPhysicalLifecycleSleepFlag(PhysicalLifecycleState* p_state, void*)
{
	// Retail tests the x87 carry bit alone: unordered values also pass.
	if ((p_state->m_base.m_stateFlags & 2) != 0 &&
		!(p_state->m_linearVelocity.m_x * p_state->m_linearVelocity.m_x +
			  p_state->m_linearVelocity.m_y * p_state->m_linearVelocity.m_y +
			  p_state->m_linearVelocity.m_z * p_state->m_linearVelocity.m_z >=
		  0.01f) &&
		!(p_state->m_worldAngularVelocity.m_x * p_state->m_worldAngularVelocity.m_x +
			  p_state->m_worldAngularVelocity.m_y * p_state->m_worldAngularVelocity.m_y +
			  p_state->m_worldAngularVelocity.m_z * p_state->m_worldAngularVelocity.m_z >=
		  0.5f)) {
		if ((p_state->m_base.m_owner->m_flags & 2) != 2) {
			p_state->m_base.m_owner->m_flags |= 2;
		}
	}
	p_state->m_base.m_stateFlags &= ~2u;
}

// STUB: LEGORACERS2 0x004a67b0
LegoU32 __fastcall IntegratePhysicalLifecycleMotion(PhysicalLifecycleState* p_state, void*)
{
	if (p_state->m_parent != NULL) {
		return 0;
	}
	ApplyPhysicalLifecycleSleepFlag(p_state, NULL);
	if ((p_state->m_base.m_owner->m_flags & 0x12) != 0 || (p_state->m_base.m_stateFlags & 1) != 0) {
		return 0;
	}
	LegoFloat step = g_currentSimulationStep;
	p_state->m_position.m_x = step * p_state->m_linearVelocity.m_x + p_state->m_position.m_x;
	p_state->m_position.m_y = step * p_state->m_linearVelocity.m_y + p_state->m_position.m_y;
	p_state->m_position.m_z = step * p_state->m_linearVelocity.m_z + p_state->m_position.m_z;
	step = g_currentSimulationStep;
	p_state->m_linearMomentum.m_x = step * p_state->m_force.m_x + p_state->m_linearMomentum.m_x;
	p_state->m_linearMomentum.m_y = step * p_state->m_force.m_y + p_state->m_linearMomentum.m_y;
	p_state->m_linearMomentum.m_z = step * p_state->m_force.m_z + p_state->m_linearMomentum.m_z;
	step = g_currentSimulationStep;
	p_state->m_angularMomentum.m_x = step * p_state->m_torque.m_x + p_state->m_angularMomentum.m_x;
	p_state->m_angularMomentum.m_y = step * p_state->m_torque.m_y + p_state->m_angularMomentum.m_y;
	p_state->m_angularMomentum.m_z = step * p_state->m_torque.m_z + p_state->m_angularMomentum.m_z;
	Quaternion velocity = {
		p_state->m_worldAngularVelocity.m_x,
		p_state->m_worldAngularVelocity.m_y,
		p_state->m_worldAngularVelocity.m_z,
		0.0f
	};
	velocity.MultiplyRight(&p_state->m_orientation);
	LegoFloat halfStep = g_currentSimulationStep * 0.5f;
	velocity.m_x *= halfStep;
	velocity.m_y *= halfStep;
	velocity.m_z *= halfStep;
	velocity.m_w *= halfStep;
	LegoFloat halfStepSquared = g_currentSimulationStep * g_currentSimulationStep * 0.5f;
	LegoFloat forceScale = halfStepSquared * p_state->m_inverseMass;
	p_state->m_position.m_x = forceScale * p_state->m_force.m_x + p_state->m_position.m_x;
	p_state->m_position.m_y = forceScale * p_state->m_force.m_y + p_state->m_position.m_y;
	p_state->m_position.m_z = forceScale * p_state->m_force.m_z + p_state->m_position.m_z;
	Vector3 angularAcceleration = p_state->m_torque;
	angularAcceleration.FUN_004a4e00(&p_state->m_inverseInertiaTensor);
	Quaternion acceleration = {angularAcceleration.m_x, angularAcceleration.m_y, angularAcceleration.m_z, 0.0f};
	acceleration.MultiplyRight(&p_state->m_orientation);
	LegoFloat angularScale = halfStepSquared * 0.5f;
	acceleration.m_x *= angularScale;
	acceleration.m_y *= angularScale;
	acceleration.m_z *= angularScale;
	acceleration.m_w *= angularScale;
	p_state->m_orientation.m_x = acceleration.m_x + p_state->m_orientation.m_x;
	p_state->m_orientation.m_y = acceleration.m_y + p_state->m_orientation.m_y;
	p_state->m_orientation.m_z = acceleration.m_z + p_state->m_orientation.m_z;
	p_state->m_orientation.m_w = acceleration.m_w + p_state->m_orientation.m_w;
	p_state->m_orientation.m_x = velocity.m_x + p_state->m_orientation.m_x;
	p_state->m_orientation.m_y = velocity.m_y + p_state->m_orientation.m_y;
	p_state->m_orientation.m_z = velocity.m_z + p_state->m_orientation.m_z;
	p_state->m_orientation.m_w = velocity.m_w + p_state->m_orientation.m_w;
	p_state->m_orientation.Normalize();
	p_state->m_rotation.FUN_004a4d30(&p_state->m_orientation.m_x);
	p_state->RebuildInverseInertiaTensor();
	p_state->UpdateAndClampVelocities();
	return 1;
}
