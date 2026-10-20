#include "memory/memoryheap.h"
#include "physics/dynamics/cachedconstraint.h"

#include <math.h>

extern LegoFloat g_contactResponseThreshold;

static CachedConstraint* __fastcall DeleteConstraint(CachedConstraint*, void*, LegoU32);
static LegoBool32 __fastcall IsContactConstraintActive(CachedConstraint*, void*)
{
	// Retail 0x00403920 returns one without reading the receiver.
	return TRUE;
}
static LegoU32 __fastcall PrepareContactConstraint(CachedConstraint* p_constraint, void*)
{
	return ((CachedCollisionContact*) p_constraint)->PrepareContact();
}
static LegoU32 __fastcall UpdateContactConstraint(CachedConstraint* p_constraint, void*)
{
	return ((CachedCollisionContact*) p_constraint)->UpdateContact();
}

// Retail base/inert tables have exactly one pointer each, followed by floats.
const ConstraintBaseDispatch g_constraintBaseDispatch = {DeleteConstraint};  // 0x00531adc
const ConstraintBaseDispatch g_inertConstraintDispatch = {DeleteConstraint}; // 0x005368e0
const ContactConstraintDispatch g_contactConstraintDispatch =
	{{DeleteConstraint}, IsContactConstraintActive, PrepareContactConstraint, UpdateContactConstraint}; // 0x00536ca4

CachedCollisionContact::CachedCollisionContact()
{
	m_prepared = 0;
	m_vtable = &g_inertConstraintDispatch;
	m_vtable = &g_contactConstraintDispatch.m_base;
}

// STUB: LEGORACERS2 0x0044b3b0
static CachedConstraint* __fastcall DeleteConstraint(CachedConstraint* p_constraint, void*, LegoU32 p_flags)
{
	p_constraint->~CachedConstraint();
	if (p_flags & 1) {
		FreeHeapMemory(p_constraint);
	}
	return p_constraint;
}

// STUB: LEGORACERS2 0x004fba30
LegoU32 CachedCollisionContact::PrepareContact()
{
	if (IsInactive()) {
		return 0;
	}
	for (LegoS32 i = 0; i < 2; ++i) {
		m_endpoints[i].UpdateWorldOffset();
		m_endpoints[i].UpdatePointAcceleration();
	}
	RebuildEffectiveMassMatrix(0);
	m_prepared = 1;
	m_force.m_x = 0;
	m_force.m_y = 0;
	m_force.m_z = 0;
	return 0;
}

// STUB: LEGORACERS2 0x004fbab0
LegoU32 CachedCollisionContact::UpdateContact()
{
	if (IsInactive()) {
		return 0;
	}
	if (m_prepared == 0) {
		ApplyForces(1);
	}
	m_prepared = 0;
	Vector3 difference;
	GetPositionDifference(&difference);
	LegoFloat separation =
		difference.m_x * m_normal.m_x + difference.m_z * m_normal.m_z + difference.m_y * m_normal.m_y;
	if (separation >= 0.0f) {
		Vector3 velocity;
		GetVelocityDifference(&velocity);
		Vector3 correction;
		correction.m_x = 0;
		correction.m_y = 0;
		correction.m_z = 0;
		LegoFloat normalSpeed = velocity.m_x * m_normal.m_x + velocity.m_y * m_normal.m_y + velocity.m_z * m_normal.m_z;
		Vector3 normalVelocity;
		normalVelocity.m_x = m_normal.m_x * normalSpeed;
		normalVelocity.m_y = m_normal.m_y * normalSpeed;
		normalVelocity.m_z = m_normal.m_z * normalSpeed;
		velocity.m_x -= normalVelocity.m_x;
		velocity.m_y -= normalVelocity.m_y;
		velocity.m_z -= normalVelocity.m_z;
		if (normalSpeed < g_contactResponseThreshold) {
			Vector3 acceleration;
			GetAccelerationDifference(&acceleration);
			LegoFloat normalAcceleration =
				acceleration.m_x * m_normal.m_x + acceleration.m_z * m_normal.m_z + acceleration.m_y * m_normal.m_y;
			if (normalAcceleration < 0.0f) {
				normalAcceleration = -normalAcceleration;
				correction.m_x = m_normal.m_x * normalAcceleration;
				correction.m_y = m_normal.m_y * normalAcceleration;
				correction.m_z = m_normal.m_z * normalAcceleration;
			}
		}
		LegoFloat correctionScale = (0.03f * 0.5f - separation) * -16.0f;
		Vector3 damping;
		if (normalSpeed >= g_contactResponseThreshold) {
			LegoFloat dampingScale = g_contactResponseThreshold * -7.0f;
			damping.m_x = m_normal.m_x * dampingScale;
			damping.m_y = m_normal.m_y * dampingScale;
			damping.m_z = m_normal.m_z * dampingScale;
		}
		else {
			damping.m_x = normalVelocity.m_x * -7.0f;
			damping.m_y = normalVelocity.m_y * -7.0f;
			damping.m_z = normalVelocity.m_z * -7.0f;
		}
		correction.m_x = damping.m_x + m_normal.m_x * correctionScale + correction.m_x;
		correction.m_y = correction.m_y + m_normal.m_y * correctionScale + damping.m_y;
		correction.m_z = correctionScale * m_normal.m_z + correction.m_z + damping.m_z;
		ComputeNormalForce(&m_force, &correction, &m_normal);
		LegoFloat friction = m_endpoints[0].m_body->m_contactFriction;
		LegoFloat secondFriction = m_endpoints[1].m_body->m_contactFriction;
		if (secondFriction < friction) {
			friction = secondFriction;
		}
		LegoFloat speedSquared =
			velocity.m_x * velocity.m_x + velocity.m_y * velocity.m_y + velocity.m_z * velocity.m_z;
		if (speedSquared < 0.001f) {
			ApplyForces(0);
			return 0;
		}
		LegoFloat frictionScale =
			(LegoFloat) sqrt(
				(m_force.m_z * m_force.m_z + m_force.m_y * m_force.m_y + m_force.m_x * m_force.m_x) / speedSquared
			) *
			friction;
		velocity.m_x *= frictionScale;
		velocity.m_y *= frictionScale;
		velocity.m_z *= frictionScale;
		m_force.m_x -= velocity.m_x;
		m_force.m_y -= velocity.m_y;
		m_force.m_z -= velocity.m_z;
		ApplyForces(0);
	}
	return 0;
}
